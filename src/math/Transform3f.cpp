#include "Transform3f.h"
#include "Matrix4f.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const Transform3f Transform3f::IDENTITY = Transform3f(Vector3f::ZERO, Quaternion::IDENTITY,Vector3f::ONE);


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Construct an identity transform.
Transform3f::Transform3f() :
	pos(Vector3f::ZERO),
	rot(Quaternion::IDENTITY),
	scale(Vector3f::ONE)
{
}

// Construct a transform from the given position and rotation.
Transform3f::Transform3f(const Vector3f& position,
						const Quaternion& rotation,
						const Vector3f& scale) :
	pos(position),
	rot(rotation),
	scale(scale)
{
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

Matrix4f Transform3f::GetMatrix() const
{
	return Matrix4f::CreateTranslation(pos) *
		Matrix4f::CreateRotation(rot) *
		Matrix4f::CreateScale(scale);
}

Matrix4f Transform3f::GetInvMatrix() const
{
	return Matrix4f::CreateScale(1.0f / scale) *
		Matrix4f::CreateRotation(rot.GetConjugate()) *
		Matrix4f::CreateTranslation(-pos);
}


//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set to the identity transform (no transform).
void Transform3f::SetIdentity()
{
	pos = Vector3f::ZERO;
	rot = Quaternion::IDENTITY;
	scale = Vector3f::ONE;
}

// Set the position and rotation of this transform.
void Transform3f::Set(const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale)
{
	pos = _pos;
	rot = _rot;
	scale = _scale;
}
