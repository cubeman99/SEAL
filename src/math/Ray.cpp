#include "Ray.h"


//-----------------------------------------------------------------------------
// Ray constants.
//-----------------------------------------------------------------------------

const Ray Ray::ORIGINX(Vector3f::UNITX, Vector3f::ZERO); // A ray at the origin pointing in the +X direction.
const Ray Ray::ORIGINY(Vector3f::UNITY, Vector3f::ZERO); // A ray at the origin pointing in the +Y direction.
const Ray Ray::ORIGINZ(Vector3f::UNITZ, Vector3f::ZERO); // A ray at the origin pointing in the +Z direction.



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a ray with uninitialized components.
Ray::Ray()
{
}

// Construct a ray with the given direction and origin.
Ray::Ray(const Vector3f& origin, const Vector3f& direction) :
	direction(direction),
	origin(origin)
{
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return the point at the given distance along the ray.
Vector3f Ray::GetPoint(float distance) const
{
	return (origin + (direction * distance));
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

Ray& Ray::SetDirection(float x, float y, float z)
{
	direction.x = x;
	direction.y = y;
	direction.z = z;
	return *this;
}

Ray& Ray::SetOrigin(float x, float y, float z)
{
	origin.x = x;
	origin.y = y;
	origin.z = z;
	return *this;
}

// Set the direction and origin of this ray.
Ray& Ray::Set(const Vector3f& origin,const Vector3f& direction)
{
	this->direction = direction;
	this->origin = origin;
	return *this;
}

