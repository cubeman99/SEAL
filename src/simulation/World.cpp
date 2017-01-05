#include "World.h"
#include <math/MathLib.h>
#include <math/Matrix4f.h>
#include <math/Vector2f.h>
#include <map>
#include <graphics/Primitives.h>


World::World() :
	m_radius(3.0f)
{
	m_mesh = Primitives::CreateIcoSphere(1.0f, 6, true);
}

World::~World()
{
	delete m_mesh;
}

// Perform a sphere-ray intersection.
bool World::CastRay(const Ray& ray, float& distance) const
{
	Vector3f worldPos = Vector3f::ZERO;

	// Setup a quadratic equation: ax^2 + bx + c.
	float a = 1.0f;
	float b = 2.0f * ray.direction.Dot(ray.origin - worldPos);
	float c = worldPos.LengthSquared() + ray.origin.LengthSquared() -
		(2.0f * worldPos.Dot(ray.origin)) - (m_radius * m_radius);

	// Solve the quadratic equation.
	float discriminant = b*b - 4*a*c;
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

