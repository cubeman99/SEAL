#ifndef _AABB_H_
#define _AABB_H_

#include <math/Vector3f.h>

struct Sphere;


//-----------------------------------------------------------------------------
// AABB - Axis Aligned Bounding Box
//-----------------------------------------------------------------------------
struct AABB
{
public:
	Vector3f mins;
	Vector3f maxs;
	
	// Constructors
	AABB();
	AABB(const Vector3f& mins, const Vector3f& maxs);

	// Getters
	Vector3f GetCenter() const;
	Vector3f GetSize() const;

	// Operations
	bool Intersects(const AABB& other) const;
	bool Intersects(const Sphere& sphere) const;


private:
};


#endif // _AABB_H_