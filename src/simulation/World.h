#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <math/Vector3f.h>
#include <math/Ray.h>
#include <graphics/Mesh.h>


//-----------------------------------------------------------------------------
// World - The spherical planet where on the surface of which the simulation 
//         components are situated.
//-----------------------------------------------------------------------------
class World
{
public:
	World();
	~World();

	inline float GetRadius() { return m_radius; }
	inline Mesh* GetMesh() { return m_mesh; }

	bool CastRay(const Ray& ray, float& distance) const;


private:
	float m_radius;
	Mesh* m_mesh;
};


#endif // _WORLD_H_