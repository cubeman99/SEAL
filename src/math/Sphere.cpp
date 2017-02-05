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
	Vector3f worldPos = Vector3f::ZERO;

	// Setup a quadratic equation: ax^2 + bx + c.
	float a = 1.0f;
	float b = 2.0f * ray.direction.Dot(ray.origin - worldPos);
	float c = worldPos.LengthSquared() + ray.origin.LengthSquared() -
		(2.0f * worldPos.Dot(ray.origin)) - (radius * radius);

	// Solve the quadratic equation.
	float discriminant = (b * b) - (4 * a * c);
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

