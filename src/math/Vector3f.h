#ifndef _CM_VECTOR_3F_H_
#define _CM_VECTOR_3F_H_

#include <iostream>

struct Vector2f;
struct Matrix3f;
struct Matrix4f;
struct Quaternion;
//struct Point3i;


// 3D floating-point vector.
struct Vector3f
{
	float x, y, z;


	// Constants.
	static const Vector3f ZERO;		// (0, 0, 0)
	static const Vector3f ONE;		// (1, 1, 1)
	static const Vector3f UNITX;	// (1, 0, 0)
	static const Vector3f UNITY;	// (0, 1, 0)
	static const Vector3f UNITZ;	// (0, 0, 1)
	static const Vector3f NEG_UNITX;	// (-1, 0, 0)
	static const Vector3f NEG_UNITY;	// (0, -1, 0)
	static const Vector3f NEG_UNITZ;	// (0, 0, -1)
	
	static const Vector3f RIGHT;	// +X
	static const Vector3f LEFT;		// -X
	static const Vector3f UP;		// +Y
	static const Vector3f DOWN;		// -Y
	static const Vector3f BACK;		// +Z
	static const Vector3f FORWARD;	// -Z

	// Constructors.
	Vector3f();
	explicit Vector3f(float x, float y, float z);
	explicit Vector3f(const Vector2f& vec2, float z);
	explicit Vector3f(float value);
	//Vector3f(const Point3i& p);

	// Accessors.
	float		operator [](int index) const;
	float&	operator [](int index);
	Vector2f	GetXY() const;
	Vector2f	GetXZ() const;
	Vector2f	GetYZ() const;
	Vector2f	GetYX() const;
	Vector2f	GetZX() const;
	Vector2f	GetZY() const;
	float		Length() const;
	float		LengthSquared() const;
	float		Dot(const Vector3f& vOther) const;
	Vector3f	Cross(const Vector3f& vOther) const;
	float		DistTo(const Vector3f& vOther) const;
	float		DistToSqr(const Vector3f& vOther) const;

	float* data();
	const float* data() const;

	// Mutators.
	Vector3f&	SetZero();
	Vector3f&	Set(float x, float y, float z);
	Vector3f&	Fill(float value);
	Vector3f&	SetXY(const Vector2f& vXY);
	Vector3f&	Negate();
	Vector3f&	Normalize();
	Vector3f&	Rotate(const Quaternion& qRotation);
	Vector3f&	Rotate(const Vector3f& vAxis, float angle);
	Vector3f&	Rotate(const Matrix4f& mRotation);
	//Vector3f&	Transform(const Matrix3f& mTransform);
	Vector3f&	Transform(const Matrix4f& mTransform);

	// Unary operators.
	Vector3f	operator -() const;
	void		operator +=(const Vector3f& vVec);
	void		operator -=(const Vector3f& vVec);
	void		operator *=(const Vector3f& vVec);
	void		operator /=(const Vector3f& vVec);
	void		operator *=(float scalar);
	void		operator /=(float denominator);

	// Binary operators.
	Vector3f	operator +(const Vector3f& other) const;
	Vector3f	operator -(const Vector3f& other) const;
	Vector3f	operator *(const Vector3f& other) const;
	Vector3f	operator /(const Vector3f& other) const;
	Vector3f	operator *(float scalar) const;
	Vector3f	operator /(float invScalar) const;

	// Static methods.
	static Vector3f	Normalize(const Vector3f& v);									// Returns a normalized vector.
	static float	Dist(const Vector3f& a, const Vector3f& b);						// Returns the distance between two vectors.
	static float	DistSqr(const Vector3f& a, const Vector3f& b);					// Returns the sqaured distance between two vectors.
	static float	Dot(const Vector3f& a, const Vector3f& b);						// Returns the dot product of two vectors.
	static Vector3f Cross(const Vector3f& a, const Vector3f& b);					// Returns cross product of two vectors.
	static Vector3f Lerp(const Vector3f& a, const Vector3f& b, float lerpFactor);	// Performs a linear interpolation between two vectors.
	static Vector3f Rotate(const Vector3f& v, const Quaternion& rotation);			// Return a vector rotated by a quaternion.
	static Vector3f Rotate(const Vector3f& v, const Vector3f axis, float angle);	// Return a vector rotated by a an axis-angle rotation (right-handed).
};


// Left-hand operators.
Vector3f		operator *(float scalar, const Vector3f& v);
Vector3f		operator /(float numerator, const Vector3f& v);
std::ostream&	operator <<(std::ostream &out, const Vector3f& v);


#endif // _CM_VECTOR_3F_H_
