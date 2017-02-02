#include "Simulation.h"
#include <utilities/Timing.h>

Simulation::Simulation() :
	m_agentSystem(this),
	m_plantSystem(this, 0.2f, 100.0f)
{
	m_tickRate = 60;
	m_timeStep = 1 / m_tickRate;
}


void Simulation::Initialize()
{
	// Initialize simulation state.
	m_ageInTicks = 0;

	// Setup the OctTree.
	AABB octTreeBounds;
	Vector3f worldPos = Vector3f::ZERO;
	Vector3f worldRadius(m_world.GetRadius() * 1.2f); // a bit larger than the world
	octTreeBounds.mins = worldPos - worldRadius;
	octTreeBounds.maxs = worldPos + worldRadius;
	m_octTree.Clear();
	m_octTree.SetBounds(octTreeBounds);
	m_octTree.SetMaxDepth(4);
	m_octTree.SetMaxObjectsPerNode(1);

	// TODO: initialize agent system and plant system.
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_agentSystem.UpdateAgents(m_timeStep);
	m_plantSystem.UpdatePlants(m_timeStep);
	
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}

