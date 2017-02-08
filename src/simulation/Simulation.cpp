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

	// Initialize systems.
	m_world.Initialize(config.world.radius);
	m_objectManager.Initialize();

	// Seed the random number generator.
	if (config.world.seed < 0)
		m_random.SeedTime();
	else
		m_random.SetSeed((unsigned long) config.world.seed);
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects();
	
	// Measure elapsed time.
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}

