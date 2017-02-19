#include "Rect2i.h"
#include <math/MathLib.h>


//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------

const Rect2i Rect2i::ZERO = Rect2i(0, 0, 0, 0);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a rectangle with uninitialized components.
Rect2i::Rect2i()
{
	
}

// Construct a rectangle from the given dimensions.
Rect2i::Rect2i(int x, int y, int width, int height) :
	position(x, y),
	size(width, height)
{
}

// Construct a rectangle from the given dimensions.
Rect2i::Rect2i(const Point2i& position, const Point2i& size) :
	position(position),
	size(size)
{
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return true if the given vector is contained in this rectangle.
bool Rect2i::Contains(const Point2i& v) const
{
	return (v.x >= position.x && v.y >= position.y &&
		v.x < position.x + size.x && v.y < position.y + size.y);
}

// Return true if the given rectangle is contained in this rectangle.
bool Rect2i::Contains(const Rect2i& r) const
{
	return (r.GetLeft() >= GetLeft() && r.GetTop() >= GetTop() &&
		r.GetRight() <= GetRight() && r.GetBottom() <= r.GetBottom());
}

// Return true if a given rectangle instersects with this rectangle.
bool Rect2i::Intersects(const Rect2i& r) const
{
	return !(r.GetLeft() - GetRight() >= 0 || r.GetTop() - GetBottom() >= 0 ||
		GetLeft() - r.GetRight() >= 0 || GetTop() - r.GetBottom() >= 0);
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the dimensions of this rectangle.
Rect2i& Rect2i::Set(int x, int y, int width, int height)
{
	position.Set(x, y);
	size.Set(width, height);
	return *this;
}

// Set the dimensions of this rectangle.
Rect2i& Rect2i::Set(const Point2i& position, const Point2i& size)
{
	this->position = position;
	this->size = size;
	return *this;
}

// Inset the rectangle's edges in each direction by the given amount.
Rect2i& Rect2i::Inset(int left, int top, int right, int bottom)
{
	position.x += left;
	position.y += top;
	size.x -= left + right;
	size.y -= top + bottom;
	return *this;
}

// Inset the rectangle's edges in each direction by the given amount.
Rect2i& Rect2i::Inset(int horizontal, int vertical)
{
	position.x += horizontal;
	position.y += vertical;
	size.x     -= 2 * horizontal;
	size.y     -= 2 * vertical;
	return *this;
}

// Expand the rectangle's edges in each direction by the given amount.
Rect2i& Rect2i::Inflate(int left, int top, int right, int bottom)
{
	position.x -= left;
	position.y -= top;
	size.x     += left + right;
	size.y     += top + bottom;
	return *this;
}

// Expand the rectangle's edges in each direction by the given amount.
Rect2i& Rect2i::Inflate(int horizontal, int vertical)
{
	position.x -= horizontal;
	position.y -= vertical;
	size.x     += 2 * horizontal;
	size.y     += 2 * vertical;
	return *this;
}

// Translate the rectangle's position.
Rect2i& Rect2i::Translate(int x, int y)
{
	position.x += x;
	position.y += y;
	return *this;
}

// Translate the rectangle's position.
Rect2i& Rect2i::Translate(const Point2i& amount)
{
	position += amount;
	return *this;
}



//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------

// Return the union of two rectangles.
Rect2i Rect2i::GetUnion(const Rect2i& a, const Rect2i& b)
{
	int x1 = Math::Min(a.GetLeft(),   b.GetLeft());
	int y1 = Math::Min(a.GetTop(),    b.GetTop());
	int x2 = Math::Max(a.GetRight(),  b.GetRight());
	int y2 = Math::Max(a.GetBottom(), b.GetBottom());
	return Rect2i(x1, y1, x2 - x1, y2 - y1);
}

// Return the overlapping portion of two rectangles,
// or the Zero rectangle if they do not intersect.
Rect2i Rect2i::GetIntersect(const Rect2i& a, const Rect2i& b)
{
	int x1 = Math::Max(a.GetLeft(),   b.GetLeft());
	int y1 = Math::Max(a.GetTop(),    b.GetTop());
	int x2 = Math::Min(a.GetRight(),  b.GetRight());
	int y2 = Math::Min(a.GetBottom(), b.GetBottom());
	if (x2 > x1 && y2 > y1)
		return Rect2i(x1, y1, x2 - x1, y2 - y1);
	return ZERO;
}

