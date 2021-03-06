#ifndef _TRANSFORM_3F_H_
#define _TRANSFORM_3F_H_

#include "Vector2f.h"
#include "Vector3f.h"
#include "Quaternion.h"

struct Matrix4f;


// A 3D transformation (position and rotation).
struct Transform3f
{
	Vector3f   pos;
	Quaternion rot;
	Vector3f   scale;

	
	static const Transform3f IDENTITY;

	// Constructors.

	// Construct an identity transform.
	Transform3f();

	// Construct a transform from the given position and rotation.
	Transform3f(const Vector3f& position,
				const Quaternion& rotation,
				const Vector3f& scale);

	// Accessors.
	Matrix4f GetMatrix() const;
	Matrix4f GetInvMatrix() const;

	// Mutators.
	void SetIdentity();
	void Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	void Set(const Vector3f& pos, const Quaternion& rot, float scale);

	inline void SetPosition(const Vector3f& position) { this->pos = position; }
	inline void SetRotation(const Quaternion& rotation) { this->rot = rotation; }
	inline void SetScale(const Vector3f& scale) { this->scale = scale; }
	inline void SetScale(float scale) { this->scale = Vector3f(scale, scale, scale); }
};

#endif // _TRANSFORM_3F_H_