#include "Simulation.h"
#include <utilities/Timing.h>


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
	
	// Spawn some plants.
	for (int i = 0; i < m_config.plant.numPlants; ++i)
		m_objectManager.SpawnObjectRandom<Plant>();
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects();
	
	// Advance to the next generation.
	m_generationAge++;
	if (m_generationAge >= m_generationDuration)
	{
		//UpdateStatistics();
		//m_generationStats.push_back(m_statistics);

		//NextGeneration();
	}
	//if (m_ageInTicks % 30 == 0)
	{
		UpdateStatistics();
		m_generationStats.push_back(m_statistics);
	}
	
	// Measure elapsed time.
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}


//-----------------------------------------------------------------------------
// Saving & loading
//-----------------------------------------------------------------------------

// Should be called only between ticks in the simulation.
bool Simulation::SaveTimeline(std::string fileName)
{
	std::ofstream fileOut;
	fileOut.open(fileName, std::ios::out | std::ios::binary);

	if (!fileOut)
	{
		// TODO: Tell user that the file could not be opened for writing
		return false;
	}

	// Write the simulation data
	WriteSimulation(fileOut);

	// Write the number of objects
	ObjectManager* objManager = GetObjectManager();
	unsigned int numObjects = objManager->GetNumObjects();
	fileOut.write((char*)&numObjects, sizeof(unsigned int));

	// Write the object data
	for (unsigned int i = 0; i < objManager->GetNumObjects(); ++i)
	{
		objManager->GetObjByIndex(i)->Write(fileOut);
	}

	fileOut.close();

	// TODO: Tell user that the file has been saved succesffully

	return true;
}

bool Simulation::LoadTimeline(std::string fileName)
{
	std::ifstream fileIn;
	fileIn.open(fileName, std::ios::in | std::ios::binary);

	if (!fileIn)
	{
		// TODO: Tell user that the file could not be opened for reading
		return false;
	}

	// Clear out the current objects
	ObjectManager* objManager = GetObjectManager();
	objManager->ClearObjects();

	// Read the simulation data
	ReadSimulation(fileIn);

	// Get number of objects
	unsigned int numObjects;
	fileIn.read((char*)&numObjects, sizeof(unsigned int));

	// Read and create objects
	bool objectCreationGoingWell = true;
	for (unsigned int i = 0; i < numObjects && objectCreationGoingWell; ++i)
	{
		objectCreationGoingWell = objManager->SpawnObjectSerialized(fileIn);
	}

	if (!objectCreationGoingWell)
	{
		// TODO: Tell user that the file was corrupt like our government
		objManager->ClearObjects();
		return false;
	}

	fileIn.close();

	// TODO: Tell user that the file has been loaded succesffully

	return true;
}

void Simulation::ReadSimulation(std::ifstream& fileIn)
{
	unsigned long currentSeed;

	fileIn.read((char*)&currentSeed, sizeof(unsigned long));
	fileIn.read((char*)&m_originalSeed, sizeof(unsigned long));
	fileIn.read((char*)&m_ageInTicks, sizeof(unsigned int));
	fileIn.read((char*)&m_generationAge, sizeof(unsigned int));
	fileIn.read((char*)&m_generationDuration, sizeof(unsigned int));
	fileIn.read((char*)&m_config, sizeof(SimulationConfig));

	m_random.SetSeed(currentSeed);
}

void Simulation::WriteSimulation(std::ofstream& fileOut)
{
	unsigned long currentSeed = m_random.GetSeed();

	fileOut.write((char*)&currentSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_originalSeed, sizeof(unsigned long));
	fileOut.write((char*)&m_ageInTicks, sizeof(unsigned int));
	fileOut.write((char*)&m_generationAge, sizeof(unsigned int));
	fileOut.write((char*)&m_generationDuration, sizeof(unsigned int));
	fileOut.write((char*)&m_config, sizeof(SimulationConfig));
}


//-----------------------------------------------------------------------------
// Internal update mechanics
//-----------------------------------------------------------------------------

void Simulation::UpdateStatistics()
{
	SimulationStats stats;
	
	// Cumulatively add statistic values for each agent.
	unsigned int numAgents = 0;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		for (unsigned int i = 0; i < GenePosition::PHYSIOLOGICAL_GENES_COUNT; ++i)
			stats.avgGeneValues[i] += it->GetGenome()->GetGeneAsFloat(i);
		stats.avgFitness += it->GetFitness();
		stats.totalEnergy += it->GetEnergy();
		numAgents++;
	}
	
	// Divide averages by agent count.
	float invNumAgents = 1.0f / numAgents;
	for (unsigned int i = 0; i < GenePosition::PHYSIOLOGICAL_GENES_COUNT; ++i)
		stats.avgGeneValues[i] *= invNumAgents;
	stats.avgFitness *= invNumAgents;

	m_statistics = stats;
}

void Simulation::NextGeneration()
{
	m_generationAge = 0;
	m_generationIndex++;

	// Count the number of agents.
	unsigned int numAgents = 0;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		numAgents++;
	}

	// Keep mating agents to create a new population.
	Agent** newPopulation = new Agent*[numAgents];
	for (unsigned int i = 0; i < numAgents; ++i)
	{
		// Select and mate two agents.
		Agent* mommy = SelectAgent();
		Agent* daddy = SelectAgent();
		Agent* child = CreateOffspring(mommy, daddy);
		newPopulation[i] = child;
	}
	
	// Remove the old objects.
	m_objectManager.ClearObjects();
	
	// Spawn some plants.
	for (int i = 0; i < m_config.plant.numPlants; ++i)
		m_objectManager.SpawnObjectRandom<Plant>();

	// Spawn the new agents.
	for (unsigned int i = 0; i < numAgents; ++i)
	{
		Agent* agent = newPopulation[i];

		// Randomize the agent's position & orientation.
		Vector3f position;
		Quaternion orientation;
		m_objectManager.CreateRandomPositionAndOrientation(position, orientation);
		agent->SetPosition(position);
		agent->SetOrientation(orientation);
		
		// Spawn the agent into the world.
		m_objectManager.SpawnObject(agent);
	}
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
		mommy->GetGenome(), daddy->GetGenome(), this);

	Agent* child = new Agent(childGenome, 100.0f);

	return child;
}
