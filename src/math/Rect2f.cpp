#include "Rect2f.h"
#include <math/Rect2i.h>
#include <math/MathLib.h>


//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------

const Rect2f Rect2f::ZERO = Rect2f(0.0f, 0.0f, 0.0f, 0.0f);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a rectangle with uninitialized components.
Rect2f::Rect2f()
{
}

// Construct a rectangle from the given dimensions.
Rect2f::Rect2f(float x, float y, float width, float height) :
	position(x, y),
	size(width, height)
{
}

// Construct a rectangle from the given dimensions.
Rect2f::Rect2f(const Vector2f& position, const Vector2f& size) :
	position(position),
	size(size)
{
}

// Construct a floating-point rectangle from integer-based one.
Rect2f::Rect2f(const Rect2i& r) :
	position(r.position),
	size(r.size)
{
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return true if the given vector is contained in this rectangle.
bool Rect2f::Contains(const Vector2f& v) const
{
	return (v.x >= position.x && v.y >= position.y &&
		v.x < position.x + size.x && v.y < position.y + size.y);
}

// Return true if the given rectangle is contained in this rectangle.
bool Rect2f::Contains(const Rect2f& r) const
{
	return (GetLeft() >= GetLeft() && GetTop() >= GetTop() &&
		GetRight() <= GetRight() && GetBottom() <= GetBottom());
}

// Return true if a given rectangle instersects with this rectangle.
bool Rect2f::Intersects(const Rect2f& r) const
{
	return !(GetLeft() - r.GetRight() >= 0.0f || GetTop() - r.GetBottom() >= 0.0f ||
			r.GetLeft() - GetRight() >= 0.0f || r.GetTop() - GetBottom() >= 0.0f);
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the dimensions of this rectangle.
Rect2f& Rect2f::Set(float x, float y, float width, float height)
{
	position.Set(x, y);
	size.Set(width, height);
	return *this;
}

// Set the dimensions of this rectangle.
Rect2f& Rect2f::Set(const Vector2f& position, const Vector2f& size)
{
	this->position = position;
	this->size = size;
	return *this;
}

// Inset the rectangle's edges in each direction by the given amount.
Rect2f& Rect2f::Inset(float left, float top, float right, float bottom)
{
	position.x += left;
	position.y += top;
	size.x -= left + right;
	size.y -= top + bottom;
	return *this;
}

// Inset the rectangle's edges in each direction by the given amount.
Rect2f& Rect2f::Inset(float horizontal, float vertical)
{
	position.x += horizontal;
	position.y += vertical;
	size.x -= 2.0f * horizontal;
	size.y -= 2.0f * vertical;
	return *this;
}

// Expand the rectangle's edges in each direction by the given amount.
Rect2f& Rect2f::Inflate(float left, float top, float right, float bottom)
{
	position.x -= left;
	position.y -= top;
	size.x += left + right;
	size.y += top + bottom;
	return *this;
}

// Expand the rectangle's edges in each direction by the given amount.
Rect2f& Rect2f::Inflate(float horizontal, float vertical)
{
	position.x -= horizontal;
	position.y -= vertical;
	size.x += 2.0f * horizontal;
	size.y += 2.0f * vertical;
	return *this;
}

// Translate the rectangle's position.
Rect2f& Rect2f::Translate(float x, float y)
{
	position.x += x;
	position.y += y;
	return *this;
}

// Translate the rectangle's position.
Rect2f& Rect2f::Translate(const Vector2f& amount)
{
	position += amount;
	return *this;
}

Rect2f& Rect2f::Scale(float scale)
{
	position *= scale;
	size *= scale;
	return *this;
}

Rect2f& Rect2f::Scale(const Vector2f& scale)
{
	position *= scale;
	size *= scale;
	return *this;
}

Rect2f& Rect2f::Scale(float xScale, float yScale)
{
	position.x *= xScale;
	position.y *= yScale;
	size.x     *= xScale;
	size.y     *= yScale;
	return *this;
}



//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------

// Return the union of two rectangles.
Rect2f Rect2f::GetUnion(const Rect2f& a, const Rect2f& b)
{
	float x1 = Math::Min(a.GetLeft(),   b.GetLeft());
	float y1 = Math::Min(a.GetTop(),    b.GetTop());
	float x2 = Math::Max(a.GetRight(),  b.GetRight());
	float y2 = Math::Max(a.GetBottom(), b.GetBottom());
	return Rect2f(x1, y1, x2 - x1, y2 - y1);
}

// Return the overlapping portion of two rectangles,
// or the Zero rectangle if they do not intersect.
Rect2f Rect2f::GetIntersect(const Rect2f& a, const Rect2f& b)
{
	float x1 = Math::Max(a.GetLeft(),   b.GetLeft());
	float y1 = Math::Max(a.GetTop(),    b.GetTop());
	float x2 = Math::Min(a.GetRight(),  b.GetRight());
	float y2 = Math::Min(a.GetBottom(), b.GetBottom());
	if (x2 > x1 && y2 > y1)
		return Rect2f(x1, y1, x2 - x1, y2 - y1);
	return ZERO;
}

Rect2f Rect2f::Translate(const Rect2f& r, const Vector2f& amount)
{
	Rect2f result = r;
	result.Translate(amount);
	return result;
}

Rect2f Rect2f::Translate(const Rect2f& r, float x, float y)
{
	Rect2f result = r;
	result.Translate(x, y);
	return result;
}

