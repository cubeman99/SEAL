#include "Simulation.h"
#include <utilities/Timing.h>
#include <math/MathLib.h>
#include <utilities/Logging.h>


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Simulation::Simulation() :
	m_objectManager(this)
{
}

Simulation::~Simulation()
{
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------

bool Simulation::IsMatingSeason() const
{
	// Mating season happens at the end of a generation.
	return (m_generationAge >= (unsigned int)
		m_config.world.offSeasonDuration);
}


//-----------------------------------------------------------------------------
// Initialization & update
//-----------------------------------------------------------------------------

void Simulation::Initialize(const SimulationConfig& config)
{
	m_config = config;

	// Initialize simulation state.
	m_ageInTicks = 0;
	m_generationAge = 0;
	m_generationDuration = m_config.world.offSeasonDuration +
		m_config.world.matingSeasonDuration;
	m_generationIndex = 0;
	m_generationStats.clear();

	// Seed the random number generator.
	if (config.world.seed < 0)
	{
		m_random.SeedTime();
		m_originalSeed = m_random.GetSeed();
	}
	else
	{
		m_random.SetSeed((unsigned long)config.world.seed);
		m_originalSeed = (unsigned long)config.world.seed;
	}

	// Initialize systems.
	m_world.Initialize(config.world.radius);
	m_objectManager.Initialize();
	m_particleSystem.Init();
	m_fittestLists[SPECIES_HERBIVORE].Reset(
		m_config.herbivore.fittestList.numFittestAgents);
	m_fittestLists[SPECIES_CARNIVORE].Reset(
		m_config.carnivore.fittestList.numFittestAgents);
	
	// Spawn initial plants.
	for (int i = 0; i < m_config.plant.numPlants; ++i)
		m_objectManager.SpawnObjectRandom<Plant>();

	// Spawn initial agents.
	for (int i = 0; i < m_config.herbivore.population.initialAgents; ++i)
		m_objectManager.SpawnObjectRandom(new Agent(SPECIES_HERBIVORE), false);
	for (int i = 0; i < m_config.carnivore.population.initialAgents; ++i)
		m_objectManager.SpawnObjectRandom(new Agent(SPECIES_CARNIVORE), false);
}

void Simulation::OnNewSimulation()
{
	m_particleSystem.Init();
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects();
	m_particleSystem.Update();
	UpdateSteadyStateGA();

	// Advance to the next generation.
	m_generationAge++;
	if (m_generationAge >= m_generationDuration)
	{
		m_generationAge = 0;
		m_generationIndex++;
	}

	// Update statistic gathering.
	UpdateStatistics();
	if (m_ageInTicks % 60 == 0)
		m_generationStats.push_back(m_statistics);
	
	// Measure elapsed time.
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}


void Simulation::OnAgentDie(Agent* agent)
{
	m_fittestLists[agent->GetSpecies()].Update(
		agent, agent->GetFitness());

	// TODO: death statistics
}

void Simulation::AddParticles(unsigned int num, ParticleType type, Vector3f position)
{
	for (unsigned int i = 0; i < num; ++i)
	{
		m_particleSystem.AddParticle(type, position);
	}
}


void Simulation::UpdateSteadyStateGA()
{
	// Count the number of agents and add new ones
	// if population size is below the minimum.
	int numAgents[SPECIES_COUNT] = { 0, 0 };
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		numAgents[(int) it->GetSpecies()]++;
	}

	// Now generate new agents if necessary.
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (numAgents[i] < m_config.species[i].population.minAgents)
			GenerateNewAgent((Species) i);
	}
}

void Simulation::GenerateNewAgent(Species species)
{
	const SpeciesConfig& config = m_config.species[species];
	FittestList& fittestList = m_fittestLists[species];
	float totalChance = config.fittestList.mateChance +
		config.fittestList.randomChance + config.fittestList.eliteChance;
	float pick = m_random.NextFloat(0.0f, totalChance);

	// Pick a random agent genome generation method:
	//   1. mate two fittest genomes
	//   2. create a new random genome
	//   3. use an elite genome

	if (pick < config.fittestList.mateChance &&
		fittestList.IsFull())
	{
		// Mate two fittest agents.
		Genome* mommy;
		Genome* daddy;
		fittestList.PickTwoTournamentSelection(
			m_random, config.fittestList.mateTournamentSize,
			mommy, daddy);
		Genome* childGenome = Genome::SpawnChild(
			mommy, daddy, config, m_random);
		Agent* child = new Agent(childGenome, 100, species);
		m_objectManager.SpawnObjectRandom(child, true);

		// Max's are determined after OnSpawn(), through SpawnObjectRandom()
		child->SetEnergy(child->GetMaxEnergy());
		child->SetHealthEnergy(child->GetMaxEnergy());
	}
	else if (pick < config.fittestList.mateChance +
		config.fittestList.randomChance || fittestList.IsEmpty())
	{
		// Generate a new random agent.
		m_objectManager.SpawnObjectRandom(new Agent(species), true);
	}
	else
	{
		// Use an elite, unaltered agent genome from the fittest list.
		Genome* genome = fittestList.PickOneRandom(m_random);
		Agent* elite = new Agent(new Genome(*genome), 100, species);
		m_objectManager.SpawnObjectRandom(elite, true);

		// Max's are determined after OnSpawn(), through SpawnObjectRandom()
		elite->SetEnergy(elite->GetMaxEnergy());
		elite->SetHealthEnergy(elite->GetMaxEnergy());
	}
}


//-----------------------------------------------------------------------------
// Saving & loading
//-----------------------------------------------------------------------------

#define SIMULATION_FILE_MAGIC_1   'LAES'
#define SIMULATION_FILE_MAGIC_2   'RBJD'
#define SIMULATION_FILE_VERSION   1

bool Simulation::ReadSimulation(std::ifstream& fileIn)
{
	// Read the file header.
	unsigned int magic1, magic2;
	unsigned int version;
	fileIn.read((char*)&magic1, sizeof(unsigned int));
	fileIn.read((char*)&magic2, sizeof(unsigned int));
	fileIn.read((char*)&version, sizeof(unsigned int));

	// Validate header information.
	if (magic1 != (unsigned int) SIMULATION_FILE_MAGIC_1 ||
		magic2 != (unsigned int) SIMULATION_FILE_MAGIC_2)
	{
		fileIn.close();
		return false;
	}
	else if (version != SIMULATION_FILE_VERSION)
	{
		fileIn.close();
		return false;
	}

	// Read simulation information.
	unsigned long currentSeed;
	fileIn.read((char*)&currentSeed, sizeof(unsigned long));
	fileIn.read((char*)&m_originalSeed, sizeof(unsigned long));
	fileIn.read((char*)&m_ageInTicks, sizeof(unsigned int));
	fileIn.read((char*)&m_generationIndex, sizeof(unsigned int));
	fileIn.read((char*)&m_generationAge, sizeof(unsigned int));
	fileIn.read((char*)&m_generationDuration, sizeof(unsigned int));
	fileIn.read((char*)&m_config, sizeof(SimulationConfig));
	fileIn.read((char*)&m_statistics, sizeof(SimulationStats));

	// Read all generation statistics
	unsigned int numStats;
	fileIn.read((char*)&numStats, sizeof(unsigned int));
	m_generationStats.resize(numStats);
	fileIn.read((char*)m_generationStats.data(),
		numStats * sizeof(SimulationStats));
	
	// Clear out the current objects
	m_objectManager.ClearObjects();

	// Get number of objects.
	unsigned int numObjects;
	fileIn.read((char*)&numObjects, sizeof(unsigned int));

	// Read and create objects.
	bool objectCreationGoingWell = true;
	for (unsigned int i = 0; i < numObjects &&
		objectCreationGoingWell; ++i)
	{
		objectCreationGoingWell = m_objectManager.
			SpawnObjectSerialized(fileIn);
	}

	if (!objectCreationGoingWell)
	{
		// TODO: Tell user that the file was corrupt like our government
		m_objectManager.ClearObjects();
		fileIn.close();
		return false;
	}

	fileIn.close();

	// Setup the simulation state.
	m_random.SetSeed(currentSeed);

	return true;
}

bool Simulation::WriteSimulation(std::ofstream& fileOut)
{
	unsigned long currentSeed = m_random.GetSeed();

	unsigned int magic1 = SIMULATION_FILE_MAGIC_1;
	unsigned int magic2 = SIMULATION_FILE_MAGIC_2;
	unsigned int version = SIMULATION_FILE_VERSION;
	
	// Write file header information.
	fileOut.write((char*)&magic1, sizeof(unsigned int));
	fileOut.write((char*)&magic2, sizeof(unsigned int));
	fileOut.write((char*)&version, sizeof(unsigned int));
	
	// Write simulation information.
	fileOut.write((char*)&currentSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_originalSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_ageInTicks, sizeof(unsigned int));
	fileOut.write((char*)&m_generationIndex, sizeof(unsigned int));
	fileOut.write((char*)&m_generationAge, sizeof(unsigned int));
	fileOut.write((char*)&m_generationDuration, sizeof(unsigned int));
	fileOut.write((char*)&m_config, sizeof(SimulationConfig));
	fileOut.write((char*)&m_statistics, sizeof(SimulationStats));

	// Write all generation statistics.
	unsigned int numStats = m_generationStats.size();
	fileOut.write((char*)&numStats, sizeof(unsigned int));
	fileOut.write((char*)m_generationStats.data(),
		numStats * sizeof(SimulationStats));

	// Write the number of objects.
	unsigned int numObjects = m_objectManager.GetNumObjects();
	fileOut.write((char*)&numObjects, sizeof(unsigned int));

	// Write all object data.
	for (unsigned int i = 0; i < m_objectManager.GetNumObjects(); ++i)
	{
		m_objectManager.GetObjByIndex(i)->Write(fileOut);
	}

	return true;
}


//-----------------------------------------------------------------------------
// Internal update mechanics
//-----------------------------------------------------------------------------

void Simulation::UpdateStatistics()
{
	SimulationStats stats;
	
	stats.simulationAge = m_ageInTicks;

	// Cumulatively add statistic values for each agent.
	unsigned int numAgents[SPECIES_COUNT] = { 0, 0 };
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		Species spec = it->GetSpecies();
		for (unsigned int i = 0; i < GenePosition::PHYSIOLOGICAL_GENES_COUNT; ++i)
			stats.species[spec].avgGeneValue[i] += it->GetGenome()->GetGeneAsFloat(i);
		stats.species[spec].avgFitness += it->GetFitness();
		stats.species[spec].avgEnergyUsage += it->GetEnergyUsage();
		stats.species[spec].avgMoveAmount += it->GetMoveAmount();
		stats.species[spec].avgTurnAmount += Math::Abs(it->GetTurnAmount());
		stats.species[spec].totalEnergy += it->GetEnergy();
		stats.species[spec].populationSize++;
		stats.species[spec].bestFitness = Math::Max(it->GetFitness(),
			stats.species[spec].bestFitness);

		numAgents[spec]++;
	}
	
	// Divide averages by agent count.
	for (unsigned int species = 0; species < 2; ++species)
	{
		if (numAgents[species] > 0)
		{
			float invNumAgents = 1.0f / numAgents[species];
			for (unsigned int i = 0; i < GenePosition::PHYSIOLOGICAL_GENES_COUNT; ++i)
				stats.species[species].avgGeneValue[i] *= invNumAgents;
			stats.species[species].avgEnergy = stats.species[species].totalEnergy * invNumAgents;
			stats.species[species].avgEnergyUsage *= invNumAgents;
			stats.species[species].avgFitness *= invNumAgents;
			stats.species[species].avgMoveAmount *= invNumAgents;
			stats.species[species].avgTurnAmount *= invNumAgents;
			
			stats.species[species].bestFitness = Math::Max(
				m_statistics.species[species].bestFitness,
				stats.species[species].bestFitness);
		}
	}

	m_statistics = stats;
}

Agent* Simulation::SelectAgent()
{
	// Count the total fitness among all agents.
	float totalFitness = 0.0f;
	std::vector<Agent*> agents;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		totalFitness += it->GetFitness();
		agents.push_back(*it);
	}

	if (agents.empty())
		return nullptr;

	if (totalFitness == 0.0f)
	{
		return agents[m_random.NextInt() % agents.size()];
	}

	// Select the agent randomly, weighted by fitness.
	float random = m_random.NextFloat();
	float counter = 0.0f;
	for (unsigned int i = 0; i < agents.size(); ++i)
	{
		counter += agents[i]->GetFitness() / totalFitness;
		if (random <= counter)
			return agents[i];
	}

	return agents.back();
}

Agent* Simulation::CreateOffspring(Agent* mommy, Agent* daddy)
{
	// Crossover and mutate the genome.
	Genome* childGenome = Genome::SpawnChild(
		mommy->GetGenome(), daddy->GetGenome(),
		GetAgentConfig(mommy->GetSpecies()), m_random);

	Agent* child = new Agent(childGenome, 100.0f, mommy->GetSpecies());

	return child;
}
