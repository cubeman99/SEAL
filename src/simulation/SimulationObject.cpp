#include "SimulationObject.h"
#include <simulation/ObjectManager.h>


SimulationObject::SimulationObject() :
	m_isVisible(true),
	m_isDestroyed(false),
	m_isSerialized(false)
{
}

void SimulationObject::Destroy()
{
	m_isDestroyed = true;
}

Sphere SimulationObject::GetBoundingSphere()
{
	return Sphere(m_position, m_radius);
}

AABB SimulationObject::GetBoundingBox()
{
	Vector3f radiusVector(m_radius);
	AABB aabb;
	aabb.mins = radiusVector;
	aabb.mins -= radiusVector;
	aabb.maxs = radiusVector;
	aabb.maxs += radiusVector;
	return aabb;
}

ObjectManager* SimulationObject::GetObjectManager()
{
	return m_objectManager;
}

Simulation* SimulationObject::GetSimulation()
{
	return m_objectManager->GetSimulation();
}
