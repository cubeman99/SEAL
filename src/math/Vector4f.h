#ifndef _VECTOR_4F_H_
#define _VECTOR_4F_H_

#include <iostream>


struct Vector2f;
struct Vector3f;
struct Matrix4f;
struct Quaternion;


// 4D floating-point vector.
struct Vector4f
{
	float x, y, z, w;


	// Constants.
	static const Vector4f ZERO;		// (0, 0, 0, 0)
	static const Vector4f ONE;		// (1, 1, 1, 1)
	static const Vector4f UNITX;	// +X
	static const Vector4f UNITY;	// +Y
	static const Vector4f UNITZ;	// +Z
	static const Vector4f UNITW;	// +W

	// Constructors.
	Vector4f();
	Vector4f(float x, float y, float z, float w);
	Vector4f(float value);
	Vector4f(const Vector2f& vec2, float z, float w);
	Vector4f(const Vector3f& vec3, float w);

	// Accessors.
	float		operator [](int index) const;
	float&		operator [](int index);
	float		Length() const;
	float		LengthSquared() const;
	Vector3f	GetXYZ() const;

	float* data();
	const float* data() const;

	// Mutators.
	Vector4f&	SetZero();
	Vector4f&	Set(float x, float y, float z, float w);
	Vector4f&	Fill(float value);
	Vector4f&	Negate();
	Vector4f&	Normalize();
	Vector4f&	Transform(const Matrix4f& mTransform);

	// Unary operators.
	Vector4f	operator -() const;
	void		operator +=(const Vector4f& v);
	void		operator -=(const Vector4f& v);
	void		operator *=(const Vector4f& v);
	void		operator /=(const Vector4f& v);
	void		operator *=(float scalar);
	void		operator /=(float invScalar);

	// Binary operators.
	Vector4f	operator +(const Vector4f& other) const;
	Vector4f	operator -(const Vector4f& other) const;
	Vector4f	operator *(const Vector4f& other) const;
	Vector4f	operator /(const Vector4f& other) const;
	Vector4f	operator *(float scalar) const;
	Vector4f	operator /(float invScalar) const;

	// Static Methods.
	static Vector4f	Normalize(const Vector4f& v);									// Returns a normalized vector.
	static float	Dist(const Vector4f& a, const Vector4f& b);						// Returns the distance between two vectors.
	static float	DistSqr(const Vector4f& a, const Vector4f& b);					// Returns the sqaured distance between two vectors.
	static float	Dot(const Vector4f& a, const Vector4f& b);						// Returns the dot product of two vectors.
	static Vector4f	Lerp(const Vector4f& a, const Vector4f& b, float lerpFactor);	// Performs a linear interpolation between two vectors.
};


// Left-hand operators.
Vector4f		operator *(float scalar, const Vector4f& v);
Vector4f		operator /(float numerator, const Vector4f& v);
std::ostream&	operator <<(std::ostream &out, const Vector4f& v);


#endif // _VECTOR_4F_H_