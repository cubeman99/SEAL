#include "Quaternion.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "MathLib.h"


//-----------------------------------------------------------------------------
// Quaternion constants.
//-----------------------------------------------------------------------------

const Quaternion Quaternion::IDENTITY = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a quaternion with uninitialized components.
Quaternion::Quaternion()
{
}

// Construct a quaternion with the given components.
Quaternion::Quaternion(float x, float y, float z, float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}

// Contruct a quaternion from a 4d vector.
Quaternion::Quaternion(const Vector4f& v) :
	x(v.x),
	y(v.y),
	z(v.z),
	w(v.w)
{
}

// Construct a quaternion from an axis-angle rotation.
Quaternion::Quaternion(const Vector3f& vAxis, float angle)
{
	Set(vAxis, angle);
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Get a component of this quaternion.
float Quaternion::operator [](int index) const
{
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

// Modify a component of this quaternion.
float& Quaternion::operator [](int index)
{
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

// Return the length of this quaternion as a vector.
float Quaternion::Length() const
{
	return Math::Sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Return the squared-length of this quaternion as a vector.
float Quaternion::LengthSquared() const
{
	return ((x * x) + (y * y) + (z * z) + (w * w));
}

// Return the conjugate of this quaternion.
Quaternion Quaternion::GetConjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

// Return the forward vector of this quaternion.
Vector3f Quaternion::GetForward() const
{
	return Vector3f::Rotate(Vector3f::FORWARD, *this);
}

// Return the back vector of this quaternion.
Vector3f Quaternion::GetBack() const
{
	return Vector3f::Rotate(Vector3f::BACK, *this);
}

// Return the up vector of this quaternion.
Vector3f Quaternion::GetUp() const
{
	return Vector3f::Rotate(Vector3f::UP, *this);
}

// Return the down vector of this quaternion.
Vector3f Quaternion::GetDown() const
{
	return Vector3f::Rotate(Vector3f::DOWN, *this);
}

// Return the left vector of this quaternion.
Vector3f Quaternion::GetLeft() const
{
	return Vector3f::Rotate(Vector3f::LEFT, *this);
}

// Return the right vector of this quaternion.
Vector3f Quaternion::GetRight() const
{
	return Vector3f::Rotate(Vector3f::RIGHT, *this);
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this quaternion to zero.
Quaternion& Quaternion::SetIdentity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
	return *this;
}

// Set the components of this quaternion.
Quaternion& Quaternion::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

// Set this quaternion from an axis-angle.
Quaternion& Quaternion::Set(const Vector3f& vAxis, float angle)
{
	angle = -angle;

	float sinHalfAngle = Math::Sin(angle * 0.5f);
	x = vAxis.x * sinHalfAngle;
	y = vAxis.y * sinHalfAngle;
	z = vAxis.z * sinHalfAngle;
	w = Math::Cos(angle * 0.5f);
	return *this;
}

// Set this quaternion from a set of euler angles.
Quaternion& Quaternion::SetEuler(float pitch, float yaw, float roll)
{
	SetIdentity();
	Rotate(Vector3f::UNITX, pitch);
	Rotate(Vector3f::UNITY, yaw);
	Rotate(Vector3f::UNITZ, roll);
	return *this;
}

// Normalize this quaternion.
Quaternion& Quaternion::Normalize()
{
	float length = Length();
		
	if (length > 0.0f)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}

	return *this;
}

// Rotate this quaternion Q by a quaternion rotation R.
// Q = R * Q
Quaternion& Quaternion::Rotate(const Quaternion& rotation)
{
	Set((rotation.x * w) + (rotation.w * x) + (rotation.y * z) - (rotation.z * y),	// X
		(rotation.y * w) + (rotation.w * y) + (rotation.z * x) - (rotation.x * z),	// Y
		(rotation.z * w) + (rotation.w * z) + (rotation.x * y) - (rotation.y * x),	// Z
		(rotation.w * w) - (rotation.x * x) - (rotation.y * y) - (rotation.z * z));	// W
	Normalize();
	return *this;
}

// Rotate this quaternion by an axis-angle rotation.
Quaternion& Quaternion::Rotate(const Vector3f& vAxis, float angle)
{
	return Rotate(Quaternion(vAxis, angle));
}



//-----------------------------------------------------------------------------
// Quaternion to Vector operations.
//-----------------------------------------------------------------------------

void Quaternion::RotateVector(Vector3f& vVec) const
{
	RotateVector(vVec, vVec);
}

void Quaternion::RotateVector(const Vector3f& vIn, Vector3f& vOut) const
{
	Quaternion q = ((*this) * vIn) * GetConjugate();
	vOut.Set(q.x, q.y, q.z);
}




//-----------------------------------------------------------------------------
// Unary operators.
//-----------------------------------------------------------------------------

// Return the inverse of this quaternion.
Quaternion Quaternion::operator -() const
{
	return Quaternion(-x, -y, -z, -w);
}

// Add another quaternion.
void Quaternion::operator +=(const Quaternion& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

// Subtract another quaternion.
void Quaternion::operator -=(const Quaternion& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

// Multiply by a scalar.
void Quaternion::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}

// Divide by an inverse scalar.
void Quaternion::operator /=(float invScalar)
{
	float scalar = 1.0f / invScalar;
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}



//-----------------------------------------------------------------------------
// Binary operators.
//-----------------------------------------------------------------------------

// Returns the sum of two quaternions.
Quaternion Quaternion::operator +(const Quaternion& other) const
{
	return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

// Returns the difference of two quaternions.
Quaternion Quaternion::operator -(const Quaternion& other) const
{
	return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

// Multiplied by a scalar.
Quaternion Quaternion::operator *(float scalar) const
{
	return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Divided by an inverse scalar.
Quaternion Quaternion::operator /(float invScalar) const
{
	float scalar = 1.0f / invScalar;
	return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Multiply quaternions.
Quaternion Quaternion::operator *(const Quaternion& other) const
{
	return Quaternion(
		(x * other.w) + (w * other.x) + (y * other.z) - (z * other.y),	// X
		(y * other.w) + (w * other.y) + (z * other.x) - (x * other.z),	// Y
		(z * other.w) + (w * other.z) + (x * other.y) - (y * other.x),	// Z
		(w * other.w) - (x * other.x) - (y * other.y) - (z * other.z)	// W
	);
}

// Multiply a quaternion by a vector.
Quaternion Quaternion::operator *(const Vector3f& v) const
{
	return Quaternion(
			(w * v.x) + (y * v.z) - (z * v.y),	// X
			(w * v.y) + (z * v.x) - (x * v.z),	// Y
			(w * v.z) + (x * v.y) - (y * v.x),	// Z
		-(x * v.x) - (y * v.y) - (z * v.z)	// W
	);
}

// Multiplied by a scalar.
Quaternion operator *(float scalar, const Quaternion& q)
{
	return Quaternion(scalar * q.x, scalar * q.y, scalar * q.z, scalar * q.w);
}

// Return the quotient of a scalar and a quaternion.
Quaternion operator /(float numerator, const Quaternion& q)
{
	return Quaternion(numerator / q.x, numerator / q.y, numerator / q.z, numerator / q.w);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Quaternion& q)
{
	out << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
	return out;
}




//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------

// Dot product
float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

// Linear interpolation between two quaternions.
Quaternion Quaternion::Lerp(const Quaternion& qFrom, const Quaternion& qTo, float t, bool takeShortestPath)
{
	float srcFactor = 1.0f - t;
	float dstFactor = t;

	// Always take the shortest path.
	if (takeShortestPath && Quaternion::Dot(qFrom, qTo) < 0.0f)
		dstFactor = -t;

	// Compute the component-wise interpolation.
	Quaternion result(
		(qFrom.x * srcFactor) + (qTo.x * dstFactor),
		(qFrom.y * srcFactor) + (qTo.y * dstFactor),
		(qFrom.z * srcFactor) + (qTo.z * dstFactor),
		(qFrom.w * srcFactor) + (qTo.w * dstFactor));

	// Normalize the result.
	result.Normalize();
	return result;
}

// Spherical Linear Interpolation
// See http://en.wikipedia.org/wiki/Slerp
Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float lerpFactor, bool takeShortestPath)
{
	static const float SLERP_EPSILON = 0.001f;

	float cosHalfTheta = Quaternion::Dot(a, b);

	// if a = b or a = -b then theta = 0 and we can return a.
	if (Math::Abs(cosHalfTheta) >= 1.0f)
		return a;
	
	if (Math::Abs(cosHalfTheta) > 1.0f - SLERP_EPSILON)
		return Lerp(a, (cosHalfTheta < 0.0f ? -b : b), lerpFactor, false);

	float sinHalfTheta = Math::Sqrt(1.0f - (cosHalfTheta * cosHalfTheta));
	float halfTheta    = Math::ATan2(sinHalfTheta, fabsf(cosHalfTheta));

	float srcFactor;
	float dstFactor;

	if (fabsf(sinHalfTheta) < SLERP_EPSILON)
	{
		srcFactor = 0.5f;
		dstFactor = 0.5f;
	}
	else
	{
		float invSinHalfTheta = 1.0f / sinHalfTheta;
		srcFactor = invSinHalfTheta * Math::Sin(halfTheta * (1.0f - lerpFactor));
		dstFactor = invSinHalfTheta * Math::Sin(halfTheta * lerpFactor);
		if (cosHalfTheta < 0.0f)
			dstFactor = -dstFactor;
	}

	return Quaternion(
		(a.x * srcFactor) + (b.x * dstFactor),
		(a.y * srcFactor) + (b.y * dstFactor),
		(a.z * srcFactor) + (b.z * dstFactor),
		(a.w * srcFactor) + (b.w * dstFactor)
	);
}

// Compute the smallest angle between two quaternions.
float Quaternion::SmallestAngle(const Quaternion& a, const Quaternion& b)
{
	float dot = Quaternion::Dot(a, b);
	return Math::ACos((2.0f * dot * dot) - 1.0f);
}

// Create a quaternion from Euler angles (pitch, yaw, then roll).
Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll)
{
	Quaternion q;
	q.SetEuler(pitch, yaw, roll);
	return q;
}

