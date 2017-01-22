#include "Simulation.h"


Simulation::Simulation() :
	m_agentSystem(this),
	m_octTree(AABB(Vector3f(-1,-1,-1), Vector3f(1,1,1)), 4)
{
	m_tickRate = 60;
	m_timeStep = 1 / m_tickRate;
}


void Simulation::Initialize()
{
	// Setup the OctTree.
	AABB octTreeBounds;
	Vector3f worldPos = Vector3f::ZERO;
	Vector3f worldRadius(m_world.GetRadius() * 1.2f); // a bit larger than the world
	octTreeBounds.mins = worldPos - worldRadius;
	octTreeBounds.maxs = worldPos + worldRadius;

	m_octTree.Clear();
	m_octTree.SetBounds(octTreeBounds);
	m_octTree.SetMaxDepth(4);
	m_octTree.SetMaxObjectsPerNode(2);
}

void Simulation::Tick()
{
	m_agentSystem.UpdateAgents(m_timeStep);
}

