#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include <vector>
#include <math/Vector3f.h>
#include <math/Ray.h>
#include <graphics/Mesh.h>


class Primitives
{
public:

	static Mesh* CreateCube();
	static Mesh* CreateIcosahedron(float radoius, bool smooth = true);

	//static void CreateUVSphere(float radius, unsigned int rings, unsigned int segments);
	static Mesh* CreateIcoSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateHexaSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateQuadSphere(float radius, unsigned int subdivisions, bool smooth = true);
	
private:
	Primitives() {}
};


#endif // _PRIMITIVES_H_