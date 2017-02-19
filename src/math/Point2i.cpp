#include "Point2i.h"
#include <math/Vector2f.h>
#include <math/MathLib.h>


//-----------------------------------------------------------------------------
// 2D Point Constants.
//-----------------------------------------------------------------------------

const Point2i Point2i::ZERO  = Point2i(0, 0);
const Point2i Point2i::ONE   = Point2i(1, 1);
const Point2i Point2i::UNITX = Point2i(1, 0);
const Point2i Point2i::UNITY = Point2i(0, 1);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a vector with unitialized components.
Point2i::Point2i()
{
}

// Construct a vector with the given components.
Point2i::Point2i(int x, int y) :
	x(x), y(y)
{
}

// Construct a vector with the given components.
Point2i::Point2i(const Vector2f& v) :
	x((int) v.x), y((int) v.y)
{
}

// Construct a vector with all components equal to the given value.
Point2i::Point2i(int value) :
	x(value), y(value)
{
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Access a component of this vector.
int Point2i::operator [](int index) const
{
	assert(index >= 0 && index < 2);
	return (&x)[index];
}

// Modify a component of this vector.
int& Point2i::operator [](int index)
{
	assert(index >= 0 && index < 2);
	return (&x)[index];
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this vector to zero.
Point2i& Point2i::SetZero()
{
	x = 0;
	y = 0;
	return *this;
}

// Set the components of this vector.
Point2i& Point2i::Set(int x, int y)
{
	this->x = x;
	this->y = y;
	return *this;
}

// Fill all the components with the given value.
Point2i& Point2i::Fill(int value)
{
	x = value;
	y = value;
	return *this;
}

// Negate this vector.
Point2i& Point2i::Negate()
{
	x = -x;
	y = -y;
	return *this;
}



//-----------------------------------------------------------------------------
// Unary operators.
//-----------------------------------------------------------------------------

// Return the inverse of this vector.
Point2i Point2i::operator -() const
{
	return Point2i(-x, -y);
}

// Add another vector.
void Point2i::operator +=(const Point2i& v)
{
	x += v.x;
	y += v.y;
}

// Subtract another vector.
void Point2i::operator -=(const Point2i& v)
{
	x -= v.x;
	y -= v.y;
}

// Multiply by another vector.
void Point2i::operator *=(const Point2i& v)
{
	x *= v.x;
	y *= v.y;
}

// Divide by another vector.
void Point2i::operator /=(const Point2i& v)
{
	x /= v.x;
	y /= v.y;
}

// Multiply by scalar.
void Point2i::operator *=(int scalar)
{
	x *= scalar;
	y *= scalar;
}

// Divide by a denominator.
void Point2i::operator /=(int denominator)
{
	x /= denominator;
	y /= denominator;
}



//-----------------------------------------------------------------------------
// Binary operators.
//-----------------------------------------------------------------------------

Point2i Point2i::operator +(const Point2i& other) const
{
	return Point2i(x + other.x, y + other.y);
}

Point2i Point2i::operator -(const Point2i& other) const
{
	return Point2i(x - other.x, y - other.y);
}

Point2i Point2i::operator *(const Point2i& other) const
{
	return Point2i(x * other.x, y * other.y);
}

Point2i Point2i::operator /(const Point2i& other) const
{
	return Point2i(x / other.x, y / other.y);
}

Point2i Point2i::operator *(int scalar) const
{
	return Point2i(x * scalar, y * scalar);
}

Point2i Point2i::operator /(int denominator) const
{
	return Point2i(x / denominator, y / denominator);
}

Point2i operator *(int scalar, const Point2i& v)
{
	return Point2i(scalar * v.x, scalar * v.y);
}

Point2i operator /(int numerator, const Point2i& v)
{
	return Point2i(numerator / v.x, numerator / v.y);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Point2i& v)
{
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}


/*

// Returns a vector multiplied by a scalar.
Vector2f operator *(const Point2i& a, float scalar)
{
	return Vector2f((float) a.x * scalar, (float) a.y * scalar);
}

// Returns a vector multiplied by a scalar.
Vector2f operator *(float scalar, const Point2i& a)
{
	return Vector2f((float) a.x * scalar, (float) a.y * scalar);
}

// Returns a vector divided by a denominator.
Vector2f operator /(const Point2i& v, float denominator)
{
	return Vector2f((float) v.x / denominator, (float) v.y / denominator);
}

// Returns the quotient of a numerater and a vector.
Vector2f operator /(float numerator, const Point2i& v)
{
	return Vector2f(numerator / (float) v.x, numerator / (float) v.y);
}

*/

