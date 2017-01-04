#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <math/Vector3f.h>
#include <math/Ray.h>


class World
{
public:
	World();
	~World();

	inline float GetRadius() { return m_radius; }
	inline std::vector<Vector3f>& GetVertices() { return m_vertices; }

	bool CastRay(const Ray& ray, float& distance) const;

private:
	float m_radius;
	

	std::vector<Vector3f> m_vertices;
};


#endif // _WORLD_H_