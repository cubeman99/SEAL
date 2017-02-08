#include "Sphere.h"
#include <math/MathLib.h>
#include <math/Ray.h>


Sphere::Sphere()
{
}

Sphere::Sphere(const Vector3f& position, float radius) :
	position(position),
	radius(radius)
{
}

bool Sphere::Contains(const Vector3f& point) const
{
	return (point.DistToSqr(position) <= radius * radius);
}

bool Sphere::Intersects(const Sphere& other) const
{
	float distSqr = position.DistToSqr(other.position);
	float radiusSumSqr = (radius * radius) + (other.radius * other.radius);
	return (distSqr < radiusSumSqr);
}

// Perform a sphere-ray intersection.
bool Sphere::CastRay(const Ray& ray, float& distance) const
{
	// Algorithm obtained from:
	// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection

	// Setup the coefficients for a quadratic equation.
	Vector3f rayPos = ray.origin - position;
	float a = ray.direction.LengthSquared();
	float b = 2.0f * (ray.direction.Dot(rayPos));
	float c = rayPos.LengthSquared() - (radius * radius);

	// Solve the quadratic equation.
	float discriminant = (b * b) - (4.0f * a * c);
	if (discriminant < 0.0f)
	{
		return false;
	}
	else
	{
		distance = (-b - Math::Sqrt(discriminant)) / (2.0f * a);
		return true;
	}
}

