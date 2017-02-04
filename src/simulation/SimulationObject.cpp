#include "SimulationObject.h"


SimulationObject::SimulationObject() :
	m_isVisible(true),
	m_isDestroyed(false)
{
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

