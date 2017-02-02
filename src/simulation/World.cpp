#include "World.h"
#include <math/MathLib.h>
#include <math/Matrix4f.h>
#include <math/Vector2f.h>
#include <map>
#include <graphics/Primitives.h>


World::World() :
	m_radius(0.8f),
	m_mesh(nullptr)
{
	//m_mesh = Primitives::CreateIcoSphere(1.0f, 6, true);
}

World::~World()
{
	//delete m_mesh;
}

// Perform a sphere-ray intersection.
bool World::CastRay(const Ray& ray, float& distance) const
{
	Sphere sphere(Vector3f::ZERO, m_radius);
	return sphere.CastRay(ray, distance);
}


