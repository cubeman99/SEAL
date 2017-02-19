#include "Simulation.h"
#include <utilities/Timing.h>


Simulation::Simulation() :
	m_objectManager(this)
{
}

Simulation::~Simulation()
{
}


void Simulation::Initialize(const SimulationConfig& config)
{
	m_config = config;
	
	// Initialize simulation state.
	m_ageInTicks = 0;
	m_generationAge = 0;
	m_generationDuration = 60 * 30;

	// Initialize systems.
	m_world.Initialize(config.world.radius);
	m_objectManager.Initialize();

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
	
	// Spawn some plants.
	for (int i = 0; i < m_config.plant.numPlants; ++i)
		m_objectManager.SpawnObjectRandom<Plant>();

	// TODO: TEMP:
	LoadTimeline("DebugTimeline");
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects();
	
	m_generationAge++;
	if (m_generationAge >= m_generationDuration)
	{
		NextGeneration();
	}

	// Measure elapsed time.
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}

void Simulation::NextGeneration()
{
	m_generationAge = 0;

	// Count the number of agents.
	unsigned int numAgents = 0;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		numAgents++;
	}

	// Keep mating agents to create a new population.
	std::vector<Agent*> newPopulation;
	for (unsigned int i = 0; i < numAgents; ++i)
	{
		// Select two agents to be the parents.
		Agent* mommy = SelectAgent();
		Agent* daddy = SelectAgent();

		// Mate the two agents to create a child agent.
		Genome* childGenome = Genome::SpawnChild(
			mommy->GetGenome(), daddy->GetGenome(), this);
		Agent* child = new Agent(childGenome, 100.0f);

		newPopulation.push_back(child);
	}
	
	// Remove the old agents.
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		it->Destroy();
	}

	// Insert the new agents.
	for (unsigned int i = 0; i < newPopulation.size(); ++i)
	{
		Agent* agent = newPopulation[i];

		Vector3f position;
		Quaternion orientation;
		m_objectManager.CreateRandomPositionAndOrientation(position, orientation);
		agent->SetPosition(position);
		agent->SetOrientation(orientation);
		
		m_objectManager.SpawnObject(agent);
	}

	// TODO: TEMP:
	// Save a copy of the timeline state
	SaveTimeline("DebugTimeline");
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

// Should be called only between ticks in the simulation.
bool Simulation::SaveTimeline(std::string fileName)
{
	std::ofstream fileOut;
	std::string filePath = "../../assets/timelines/" + fileName + ".bin";
	fileOut.open(filePath, std::ios::out | std::ios::binary);

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
	std::string filePath = "../../assets/timelines/" + fileName + ".bin";
	fileIn.open(filePath, std::ios::in | std::ios::binary);

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
		//objManager->ClearObjects();
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
