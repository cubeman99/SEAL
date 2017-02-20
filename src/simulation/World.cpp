#include "World.h"
#include <math/MathLib.h>
#include <math/Matrix4f.h>
#include <math/Vector2f.h>
#include <map>
#include <graphics/Primitives.h>


World::World() :
	m_radius(0.8f)
{
}

World::~World()
{
}

// Initialize the world with the given radius.
void World::Initialize(float radius)
{
	m_radius = radius;
}

// Cast a ray onto the sphere, getting the distance between the contact
// point. Returns true if the ray cast hit the sphere.
bool World::CastRay(const Ray& ray, float& distance) const
{
	Sphere sphere(Vector3f::ZERO, m_radius);
	return sphere.CastRay(ray, distance);
}

