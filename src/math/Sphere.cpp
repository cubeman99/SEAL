#include "Sphere.h"



Sphere::Sphere()
{
}

Sphere::Sphere(const Vector3f& position, float radius) :
	position(position),
	radius(radius)
{
}

bool Sphere::Intersects(const Sphere& other) const
{
	float distSqr = position.DistToSqr(other.position);
	float radiusSumSqr = (radius * radius) + (other.radius * other.radius);
	return (distSqr < radiusSumSqr);
}

