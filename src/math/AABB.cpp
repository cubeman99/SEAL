#include "AABB.h"
#include "Sphere.h"
#include "MathLib.h"


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

bool AABB::Intersects(const Sphere& sphere) const
{
	// Clamp the sphere's position to the AABB bounds.
	Vector3f p;
	p.x = Math::Clamp(sphere.position.x, mins.x, maxs.x);
	p.y = Math::Clamp(sphere.position.y, mins.y, maxs.y);
	p.z = Math::Clamp(sphere.position.z, mins.z, maxs.z);

	// Check if the clamped point is within the sphere's radius.
	return (p.DistToSqr(sphere.position) < sphere.radius * sphere.radius);
}
