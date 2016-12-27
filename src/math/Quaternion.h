#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <iostream>

struct Vector3f;
struct Vector4f;


// Half-angle quaternion (represents a 3D rotation).
struct Quaternion
{
	float x, y, z, w;


	// Constants.
	static const Quaternion IDENTITY;

	// Constructors.
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(const Vector4f& v);
	Quaternion(const Vector3f& axis, float angle);

	// Accessors.
	float		operator [](int index) const;
	float&		operator [](int index);
	float		Length() const;
	float		LengthSquared() const;
	Quaternion	GetConjugate() const;
	Vector3f	GetForward() const;
	Vector3f	GetBack() const;
	Vector3f	GetUp() const;
	Vector3f	GetDown() const;
	Vector3f	GetLeft()  const;
	Vector3f	GetRight() const;

	// Mutators.
	Quaternion& SetIdentity();
	Quaternion& Set(float x, float y, float z, float w);
	Quaternion& Set(const Vector3f& axis, float angle);
	Quaternion& SetEuler(float pitch, float yaw, float roll);
	Quaternion& Normalize();
	Quaternion& Rotate(const Quaternion& rotation);
	Quaternion& Rotate(const Vector3f& axis, float angle);

	// Quaternion to Vector operations.
	void		RotateVector(Vector3f& vVec) const;
	void		RotateVector(const Vector3f& vIn, Vector3f& vOut) const;

	// Unary operators.
	Quaternion	operator -() const;
	void		operator +=(const Quaternion& q);
	void		operator -=(const Quaternion& q);
	void		operator *=(float scalar);
	void		operator /=(float invScalar);
	
	// Binary operators.
	Quaternion	operator +(const Quaternion& other) const;
	Quaternion	operator -(const Quaternion& other) const;
	Quaternion	operator *(float scalar) const;
	Quaternion	operator /(float invScalar) const;
	Quaternion	operator *(const Quaternion& other) const;
	Quaternion	operator *(const Vector3f& vVec) const;

	// Static Methods.
	static Vector3f		Normalize(const Vector3f& v);
	static float		Dot(const Quaternion& a, const Quaternion& b);
	static Quaternion	Lerp(const Quaternion& a, const Quaternion& b, float lerpFactor, bool takeShortestPath);
	static Quaternion	Slerp(const Quaternion& a, const Quaternion& b, float lerpFactor, bool takeShortestPath);
	static Quaternion	Rotate(const Vector3f& v, const Quaternion& rotation);
	static Quaternion	Rotate(const Quaternion& v, const Vector3f axis, float angle);
	static float		SmallestAngle(const Quaternion& a, const Quaternion& b);
	static Quaternion	FromEuler(float pitch, float yaw, float roll);
};


// Left-hand Operators.
Quaternion		operator *(float scalar, const Quaternion& q);
Quaternion		operator /(float numerator, const Quaternion& q);
std::ostream&	operator <<(std::ostream &out, const Quaternion& v);


#endif // _QUATERNION_H_