#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <math/Vector3f.h>


class World
{
public:
	World();
	~World();

	inline std::vector<Vector3f>& GetVertices() { return m_vertices; }

private:
	float m_radius;

	std::vector<Vector3f> m_vertices;
};


#endif // _WORLD_H_