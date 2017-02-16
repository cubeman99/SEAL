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
	m_generationDuration = 60 * 10;

	// Initialize systems.
	m_world.Initialize(config.world.radius);
	m_objectManager.Initialize();

	// Seed the random number generator.
	if (config.world.seed < 0)
		m_random.SeedTime();
	else
		m_random.SetSeed((unsigned long) config.world.seed);
	
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

	// Find the 


	
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
		//Genome* childGenome;
		//mommy->GetGenome()
			//Genome::SpawnChild(
		//Agent* child = new Agent(;

		//newPopulation.push_back(child);
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
		m_objectManager.SpawnObject(agent);
		// TODO: Randomize position.
	}
}

Agent* Simulation::SelectAgent()
{
	// Count the total fitness among all agents.
	float totalFitness = 0.0f;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		//totalFitness += it->GetFitness();
	}

	// Select the agent randomly, weighted by fitness.
	float random = m_random.NextFloat();
	float counter = 0.0f;
	for (auto it = m_objectManager.agents_begin();
		it != m_objectManager.agents_end(); ++it)
	{
		//counter += it->GetFitness();
		if (random <= counter)
			return *it;
	}

	return nullptr;
}