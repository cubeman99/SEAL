#include "Vector2f.h"
#include "MathLib.h"


//-----------------------------------------------------------------------------
// 2D Vector constants.
//-----------------------------------------------------------------------------

const Vector2f Vector2f::ZERO  = Vector2f(0.0f, 0.0f);
const Vector2f Vector2f::ONE   = Vector2f(1.0f, 1.0f);
const Vector2f Vector2f::UNITX = Vector2f(1.0f, 0.0f);
const Vector2f Vector2f::UNITY = Vector2f(0.0f, 1.0f);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a vector with uninitialized components.
Vector2f::Vector2f()
{
}

//Vector2f::Vector2f(const Point2i& v) :
//	x((float) v.x),
//	y((float) v.y)
//{
//}

// Construct a vector with the given components.
Vector2f::Vector2f(float x, float y) :
	x(x),
	y(y)
{
}
	
// Construct a vector with all components equal to the given value.
//Vector2f::Vector2f(float value) :
//	x(value),
//	y(value)
//{
//}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Get a component of this vector.
float Vector2f::operator [](int index) const
{
	assert(index >= 0 && index < 2);
	return (&x)[index];
}

// Modify a component of this vector.
float& Vector2f::operator [](int index)
{
	assert(index >= 0 && index < 2);
	return (&x)[index];
}

// Return the length of this vector.
float Vector2f::Length() const
{
	return Math::Sqrt((x * x) + (y * y));
}

// Return the squared-length of this vector.
float Vector2f::LengthSquared() const
{
	return ((x * x) + (y * y));
}

float Vector2f::Dot(const Vector2f& vOther) const
{
	return Vector2f::Dot(*this, vOther);
}

float Vector2f::Cross(const Vector2f& vOther) const
{
	return Vector2f::Cross(*this, vOther);
}

float Vector2f::DistTo(const Vector2f& vOther) const
{
	return Vector2f::Dist(*this, vOther);
}

float Vector2f::DistToSqr(const Vector2f& vOther) const
{
	return Vector2f::DistSqr(*this, vOther);
}

float* Vector2f::data()
{
	return  &x;
}

const float* Vector2f::data() const
{
	return &x;
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this vector to zero.
Vector2f& Vector2f::SetZero()
{
	x = 0.0f;
	y = 0.0f;
	return *this;
}

// Set the components of this vector.
Vector2f& Vector2f::Set(float x, float y)
{
	this->x = x;
	this->y = y;
	return *this;
}

// Set all the components of this vector to one value.
Vector2f& Vector2f::Fill(float value)
{
	x = value;
	y = value;
	return *this;
}

// Negate this vector.
Vector2f& Vector2f::Negate()
{
	x = -x;
	y = -y;
	return *this;
}

// Normalize the vector, returning a reference to it.
Vector2f& Vector2f::Normalize()
{
	float length = Length();

	if (length > 0.0f)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
	}

	return *this;
}

// Rotate this vector clockwise by an angle around the origin (0, 0).
Vector2f& Vector2f::Rotate(float angle)
{
	float cs = Math::Cos(angle);
	float sn = Math::Sin(angle);
	float tempX = x;
	x = (tempX * cs) - (y * sn); 
	y = (tempX * sn) + (y * cs);
	return *this;
}

// Rotate this vector clockwise by an angle around the given point.
Vector2f& Vector2f::Rotate(const Vector2f& origin, float angle)
{
	x -= origin.x;
	y -= origin.y;
	Rotate(angle);
	x += origin.x;
	y += origin.y;
	return *this;
}



//-----------------------------------------------------------------------------
// Unary operators.
//-----------------------------------------------------------------------------

// Return the negation of this vector.
Vector2f Vector2f::operator -() const
{
	return Vector2f(-x, -y);
}

// Add another vector.
void Vector2f::operator +=(const Vector2f& v)
{
	x += v.x; y += v.y;
}

// Subtract another vector.
void Vector2f::operator -=(const Vector2f& v)
{
	x -= v.x;
	y -= v.y;
}

// Multiply another vector.
void Vector2f::operator *=(const Vector2f& v)
{
	x *= v.x;
	y *= v.y;
}

// Divide by another vector.
void Vector2f::operator /=(const Vector2f& v)
{
	x /= v.x;
	y /= v.y;
}

// Multiply by a scalar.
void Vector2f::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
}

// Divide by the inverse of a scalar.
void Vector2f::operator /=(float invScalar)
{
	float scalar = 1.0f / invScalar;
	x *= scalar;
	y *= scalar;
}



//-----------------------------------------------------------------------------
// Binary operators.
//-----------------------------------------------------------------------------

Vector2f Vector2f::operator +(const Vector2f& other) const
{
	return Vector2f(x + other.x, y + other.y);
}

Vector2f Vector2f::operator -(const Vector2f& other) const
{
	return Vector2f(x - other.x, y - other.y);
}

Vector2f Vector2f::operator *(const Vector2f& other) const
{
	return Vector2f(x * other.x, y * other.y);
}

Vector2f Vector2f::operator /(const Vector2f& other) const
{
	return Vector2f(x / other.x, y / other.y);
}

Vector2f Vector2f::operator *(float scalar) const
{
	return Vector2f(x * scalar, y * scalar);
}

Vector2f Vector2f::operator /(float invScalar) const
{
	float scalar = 1.0f / invScalar;
	return Vector2f(x * scalar, y * scalar);
}

Vector2f operator *(float scalar, const Vector2f& v)
{
	return Vector2f(v.x * scalar, v.y * scalar);
}

Vector2f operator /(float numerator, const Vector2f& v)
{
	return Vector2f(numerator / v.x, numerator / v.y);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Vector2f& v)
{
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}



//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------

// Returns a normalized vector.
Vector2f Vector2f::Normalize(const Vector2f& v)
{
	Vector2f result = v;
	return result.Normalize();
}

// Returns the distance between two vectors.
float Vector2f::Dist(const Vector2f& a, const Vector2f& b)
{
	return Math::Sqrt(
			(b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y));
}

// Returns the sqaured distance between two vectors.
float Vector2f::DistSqr(const Vector2f& a, const Vector2f& b)
{
	return ((b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y));
}

// Returns the dot product of two vectors.
float Vector2f::Dot(const Vector2f& a, const Vector2f& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

// Returns 2D cross product of two vectors.
float Vector2f::Cross(const Vector2f& a, const Vector2f& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

// Performs a linear interpolation between two vectors.
Vector2f Vector2f::Lerp(const Vector2f& a, const Vector2f& b, float lerpFactor)
{
	return (a * (1.0f - lerpFactor)) + (b * lerpFactor);
}

