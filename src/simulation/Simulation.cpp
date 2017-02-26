#include "Simulation.h"
#include <utilities/Timing.h>
#include <math/MathLib.h>


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
	// TODO: mating season is disabled for now. We should implement
	// predator/prey first before re-enabling it.
	return true;
	//return (m_generationAge > (m_generationDuration * 0.6f));
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
	m_generationDuration = 60 * 30;
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
	m_fittestLists[0].Reset(10);
	m_fittestLists[1].Reset(10);
	
	// Spawn some plants.
	for (int i = 0; i < m_config.plant.numPlants; ++i)
		m_objectManager.SpawnObjectRandom<Plant>();

	// Spawn some agents.
	for (int i = 0; i < m_config.herbivore.population.initialAgents; ++i)
		m_objectManager.SpawnObjectRandom(new Agent(SPECIES_HERBIVORE));
	for (int i = 0; i < m_config.carnivore.population.initialAgents; ++i)
		m_objectManager.SpawnObjectRandom(new Agent(SPECIES_CARNIVORE));
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects();
	UpdateSteadyStateGA();

	// Advance to the next generation.
	m_generationAge++;
	if (m_generationAge >= m_generationDuration)
		NextGeneration();

	// Update statistic gathering.
	UpdateStatistics();
	if (m_ageInTicks % 60 == 0)
		m_generationStats.push_back(m_statistics);
	
	// Measure elapsed time.
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}

void Simulation::UpdateSteadyStateGA()
{
	// Count the number of agents and add new ones if population size is below the minimum.
	int numAgents[2] = { 0, 0 };
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		numAgents[(int) it->GetSpecies()]++;
	}

	const int tournamentSize = 5; // TODO: magic number

	for (unsigned int i = 0; i < 2; ++i)
	{
		if (numAgents[i] < m_config.species[i].population.minAgents)
		{
			// Choose method of agent generation.
			if (m_fittestLists->IsFull() && 
				m_random.NextFloat() < 0.5f) // TODO: magic number
			{
				// Mate two fittest agents.
				Genome* mommy;
				Genome* daddy;
				m_fittestLists[i].PickTwoTournamentSelection(
					tournamentSize, mommy, daddy);
				Genome* childGenome = Genome::SpawnChild(
					mommy, daddy, m_config.species[i], m_random);
				Agent* child = new Agent(childGenome, 100, (Species) i);
				child->SetEnergy(child->GetMaxEnergy());
				child->SetHealthEnergy(child->GetMaxEnergy());
				m_objectManager.SpawnObjectRandom(child);
			}
			else
			{
				// Generate a new random agent.
				m_objectManager.SpawnObjectRandom(new Agent((Species) i));
			}

			// TODO: chance of elite
		}
	}

}

//-----------------------------------------------------------------------------
// Saving & loading
//-----------------------------------------------------------------------------

void Simulation::ReadSimulation(std::ifstream& fileIn)
{
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
	m_generationStats.clear();
	unsigned int statsNum;
	fileIn.read((char*)&statsNum, sizeof(unsigned int));

	for (unsigned int i = 0; i < statsNum; ++i)
	{
		m_generationStats.push_back(SimulationStats());
		fileIn.read((char*)&m_generationStats[i], sizeof(SimulationStats));
	}

	m_random.SetSeed(currentSeed);
}

void Simulation::WriteSimulation(std::ofstream& fileOut)
{
	unsigned long currentSeed = m_random.GetSeed();

	fileOut.write((char*)&currentSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_originalSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_ageInTicks, sizeof(unsigned int));
	fileOut.write((char*)&m_generationIndex, sizeof(unsigned int));
	fileOut.write((char*)&m_generationAge, sizeof(unsigned int));
	fileOut.write((char*)&m_generationDuration, sizeof(unsigned int));
	fileOut.write((char*)&m_config, sizeof(SimulationConfig));
	fileOut.write((char*)&m_statistics, sizeof(SimulationStats));

	// Write all generation statistics
	unsigned int statsNum = m_generationStats.size();
	fileOut.write((char*)&statsNum, sizeof(unsigned int));

	for (unsigned int i = 0; i < statsNum; ++i)
	{
		fileOut.write((char*)&m_generationStats[i], sizeof(SimulationStats));
	}
}


//-----------------------------------------------------------------------------
// Internal update mechanics
//-----------------------------------------------------------------------------

void Simulation::UpdateStatistics()
{
	SimulationStats stats;
	
	stats.simulationAge = m_ageInTicks;

	// Cumulatively add statistic values for each agent.
	unsigned int numAgents[2] = { 0, 0 };
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		Species spec = it->GetSpecies();
		for (unsigned int i = 0; i < GenePosition::PHYSIOLOGICAL_GENES_COUNT; ++i)
			stats.species[spec].avgGeneValue[i] += it->GetGenome()->GetGeneAsFloat(i);
		stats.species[spec].avgFitness += it->GetFitness();
		stats.species[spec].totalEnergy += it->GetEnergy();
		stats.species[spec].avgEnergyUsage += it->GetEnergyUsage();
		stats.species[spec].avgMoveAmount += it->GetMoveAmount();
		stats.species[spec].avgTurnAmount += Math::Abs(it->GetTurnAmount());
		stats.species[spec].populationSize++;
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
		}
	}

	m_statistics = stats;
}

void Simulation::NextGeneration()
{
	// This function can still be useful for season purposes
	m_generationAge = 0;
	m_generationIndex++;

	// Old asexual reproduction code: //////////////////////

	//// Count the number of agents.
	//unsigned int numAgents = 0;
	//for (auto it = m_objectManager.agents_begin();
	//	it != m_objectManager.agents_end(); ++it)
	//{
	//	numAgents++;
	//}

	//// Keep mating agents to create a new population.
	//Agent** newPopulation = new Agent*[numAgents];
	//for (unsigned int i = 0; i < numAgents; ++i)
	//{
	//	// Select and mate two agents.
	//	Agent* mommy = SelectAgent();
	//	Agent* daddy = SelectAgent();
	//	Agent* child = CreateOffspring(mommy, daddy);
	//	newPopulation[i] = child;
	//}
	//
	//// Remove the old objects.
	//m_objectManager.ClearObjects();
	//
	//// Spawn some plants.
	//for (int i = 0; i < m_config.plant.numPlants; ++i)
	//	m_objectManager.SpawnObjectRandom<Plant>();

	//// Spawn the new agents.
	//for (unsigned int i = 0; i < numAgents; ++i)
	//{
	//	Agent* agent = newPopulation[i];

	//	// Randomize the agent's position & orientation.
	//	Vector3f position;
	//	Quaternion orientation;
	//	m_objectManager.CreateRandomPositionAndOrientation(position, orientation);
	//	agent->SetPosition(position);
	//	agent->SetOrientation(orientation);
	//	
	//	// Spawn the agent into the world.
	//	m_objectManager.SpawnObject(agent);
	//}
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
