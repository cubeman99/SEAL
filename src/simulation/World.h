#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <math/Vector3f.h>
#include <math/Ray.h>
#include <math/Sphere.h>
#include <graphics/Mesh.h>


//-----------------------------------------------------------------------------
// World - The spherical planet where on the surface of which the simulation 
//         components are situated. At the moment, this is pretty much just a
//         radius.
//
// NOTE: The world's center is always positioned at the origin (0,0,0)
//-----------------------------------------------------------------------------
class World
{
public:
	World();
	~World();

	inline float GetRadius() const { return m_radius; }


	// Initialize the world with the given radius.
	void Initialize(float radius);

	// Cast a ray onto the sphere, getting the distance between the contact
	// point. Returns true if the ray cast hit the sphere.
	bool CastRay(const Ray& ray, float& distance) const;


private:
	float m_radius;
};


#endif // _WORLD_H_