#include "SimulationObject.h"


SimulationObject::SimulationObject()
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

