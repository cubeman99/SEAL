#include "Simulation.h"
#include <utilities/Timing.h>

Simulation::Simulation() :
	m_agentSystem(this, 0.25f, 100.0f),
	m_objectManager(this)
{
	m_tickRate = 60.0f;
	m_timeStep = 1.0f / m_tickRate;
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
	m_objectManager.Initialize();
}

void Simulation::Tick()
{
	double startTime = Time::GetTime();

	// Update systems.
	m_ageInTicks++;
	m_objectManager.UpdateObjects(m_timeStep);
	//m_agentSystem.UpdateAgents(m_timeStep);
	//m_agentSystem.UpdatePlants(m_timeStep);
	
	double endTime = Time::GetTime();
	double elapsedTimeInMs = (endTime - startTime) * 1000.0;
}

