#include "Vector4f.h"
#include "Vector3f.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "MathLib.h"


//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------

const Vector4f Vector4f::ZERO  = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4f Vector4f::ONE   = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4f Vector4f::UNITX = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4f Vector4f::UNITY = Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4f Vector4f::UNITZ = Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4f Vector4f::UNITW = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);



//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Construct a vector with uninitialized components.
Vector4f::Vector4f()
{
}

// Construct a vector with the given components.
Vector4f::Vector4f(float x, float y, float z, float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}

// Construct a vector with all components equal to the given value.
Vector4f::Vector4f(float value) :
	x(value),
	y(value),
	z(value),
	w(value)
{
}

// Contruct a 4d vector from a 2d vector with specified zw-components.
Vector4f::Vector4f(const Vector2f& vec2, float z, float w) :
	x(vec2.x),
	y(vec2.y),
	z(z),
	w(w)
{
}

// Contruct a 4d vector from a 3d vector with a specified w-component.
Vector4f::Vector4f(const Vector3f& vec3, float w) :
	x(vec3.x),
	y(vec3.y),
	z(vec3.z),
	w(w)
{
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Access a component of this vector.
float Vector4f::operator[](int index) const
{
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

// Modify a component of this vector.
float& Vector4f::operator[](int index)
{
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

// Return the length of this vector.
float Vector4f::Length() const
{
	return Math::Sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Return the squared-length of this vector.
float Vector4f::LengthSquared() const
{
	return ((x * x) + (y * y) + (z * z) + (w * w));
}

// Return the xyz-components of this vector.
Vector3f Vector4f::GetXYZ() const
{
	return Vector3f(x, y, z);
}

float* Vector4f::data()
{
	return  &x;
}

const float* Vector4f::data() const
{
	return &x;
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this vector to zero.
Vector4f& Vector4f::SetZero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
	return *this;
}

// Set the components of this vector.
Vector4f& Vector4f::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

// Set the components of this vector.
Vector4f& Vector4f::Fill(float value)
{
	x = value;
	y = value;
	z = value;
	w = value;
	return *this;
}

// Negate this vector.
Vector4f& Vector4f::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

// Normalize this vector.
Vector4f& Vector4f::Normalize()
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

// Transform this vector by a 4D matrix.
Vector4f& Vector4f::Transform(const Matrix4f& mTransform)
{
	Vector4f result;
	mTransform.ApplyTransform(*this, result);
	*this = result;
	return *this;
}



//-----------------------------------------------------------------------------
// Unary operators
//-----------------------------------------------------------------------------

// Return the inverse of this vector.
Vector4f Vector4f::operator -() const
{
	return Vector4f(-x, -y, -z, -w);
}

// Add another vector.
void Vector4f::operator +=(const Vector4f& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

// Subtract another vector.
void Vector4f::operator -=(const Vector4f& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

// Multiply by another vector.
void Vector4f::operator *=(const Vector4f& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

// Divide by another vector.
void Vector4f::operator /=(const Vector4f& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

void Vector4f::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}

void Vector4f::operator /=(float invScalar)
{
	float scalar = 1.0f / invScalar;
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}



//-----------------------------------------------------------------------------
// Binary operators
//-----------------------------------------------------------------------------

Vector4f Vector4f::operator +(const Vector4f& other) const
{
	return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4f Vector4f::operator -(const Vector4f& other) const
{
	return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4f Vector4f::operator *(const Vector4f& other) const
{
	return Vector4f(x * other.x, y * other.y, z * other.z, w * other.w);
}

Vector4f Vector4f::operator /(const Vector4f& other) const
{
	return Vector4f(x / other.x, y / other.y, z / other.z, w / other.w);
}

Vector4f Vector4f::operator *(float scalar) const
{
	return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4f Vector4f::operator /(float invScalar) const
{
	float scalar = 1.0f / invScalar;
	return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Returns a vector multiplied by a scalar.
Vector4f operator *(float scalar, const Vector4f& v)
{
	return Vector4f(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

// Returns the quotient of a numerater and a vector.
Vector4f operator /(float numerator, const Vector4f& v)
{
	return Vector4f(v.x  / numerator, v.y / numerator, v.z / numerator, v.w / numerator);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Vector4f& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}



//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

// Returns a normalized vector.
Vector4f Vector4f::Normalize(const Vector4f& v)
{
	Vector4f result = v;
	result.Normalize();
	return result;
}

// Returns the distance between two vectors.
float Vector4f::Dist(const Vector4f& a, const Vector4f& b)
{
	return Math::Sqrt(
			(b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y) +
			(b.z - a.z) * (b.z - a.z) +
			(a.w - b.w) * (b.w - a.w));
}

// Returns the squared distance between two vectors.
float Vector4f::DistSqr(const Vector4f& a, const Vector4f& b)
{
	return ((b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y) +
			(b.z - a.z) * (b.z - a.z) +
			(b.w - a.w) * (b.w - a.w));
}

// Returns the dot product of two vectors.
float Vector4f::Dot(const Vector4f& a, const Vector4f& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

// Performs a linear interpolation between two vectors.
Vector4f Vector4f::Lerp(const Vector4f& a, const Vector4f& b, float lerpFactor)
{
	return (a * (1.0f - lerpFactor)) + (b * lerpFactor);
}

