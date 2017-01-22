#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <math/Vector3f.h>


struct Sphere
{
public:
	Vector3f position;
	float radius;
	
	// Constructors
	Sphere();
	Sphere(const Vector3f& position, float radius);

	// Operations
	bool Intersects(const Sphere& other) const;


private:
};


#endif // _SPHERE_H_