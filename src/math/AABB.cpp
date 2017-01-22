#include "AABB.h"


AABB::AABB()
{
}

AABB::AABB(const Vector3f& mins, const Vector3f& maxs) :
	mins(mins),
	maxs(maxs)
{
}


Vector3f AABB::GetCenter() const
{
	Vector3f center = maxs;
	center += mins;
	center *= 0.5f;
	return center;
}

Vector3f AABB::GetSize() const
{
	return (maxs - mins);
}


bool AABB::Intersects(const AABB& other) const
{
	return (maxs.x > other.mins.x && other.maxs.x > mins.x &&
			maxs.y > other.mins.y && other.maxs.y > mins.y &&
			maxs.z > other.mins.z && other.maxs.z > mins.z);
}

