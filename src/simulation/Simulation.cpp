#include "Simulation.h"


Simulation::Simulation() :
	m_agentSystem(this)
{
	m_tickRate = 60;
	m_timeStep = 1 / m_tickRate;
}


void Simulation::Tick()
{
	m_agentSystem.UpdateAgents(m_timeStep);
}

