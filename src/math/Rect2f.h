#ifndef _RECT_2F_H_
#define _RECT_2F_H_

#include <math/Vector2f.h>

struct Insets;
struct Rect2i;


// Axis-aligned, 2D floating-point rectangle.
struct Rect2f
{
	Vector2f position, size;

	
	// Constants.
	static const Rect2f ZERO;

	// Constructors.
	Rect2f();
	Rect2f(float x, float y, float width, float height);
	Rect2f(const Vector2f& position, const Vector2f& size);
	explicit Rect2f(const Rect2i& position);

	// Accessors.
	inline float	GetX()				const { return position.x; }
	inline float	GetY()				const { return position.y; }
	inline float	GetWidth()			const { return size.x; }
	inline float	GetHeight()			const { return size.y; }
	inline float	GetLeft()			const { return position.x; }
	inline float	GetTop()			const { return position.y; }
	inline float	GetRight()			const { return position.x + size.x; }
	inline float	GetBottom()			const { return position.y + size.y; }
	inline float	GetArea()			const { return size.x * size.y; }
	inline float	GetParimeter()		const { return 2.0f * (size.x + size.y); }
	inline Vector2f GetTopLeft()		const { return position; }
	inline Vector2f GetTopRight()		const { return Vector2f(position.x + size.x, position.y); }
	inline Vector2f GetBottomLeft()		const { return Vector2f(position.x, position.y + size.y); }
	inline Vector2f GetBottomRight()	const { return position + size; }
	inline Vector2f GetCenter()			const { return position + (size * 0.5f); }
	bool Contains(const Vector2f& v)    const;
	bool Contains(const Rect2f& r)      const;
	bool Intersects(const Rect2f& r)    const;

	// Mutators.
	inline void SetX(float x)				{ position.x = x; }
	inline void SetY(float y)				{ position.y = y; }
	inline void SetWidth(float width)		{ size.x = width; }
	inline void SetHeight(float height)		{ size.y = height; }
	Rect2f& Set(float x, float y, float width, float height);
	Rect2f& Set(const Vector2f& position, const Vector2f& size);
	Rect2f& Inset(float left, float top, float right, float bottom);
	Rect2f& Inset(float horizontal, float vertical);
	Rect2f& Inset(Insets insets);
	Rect2f& Inflate(float left, float top, float right, float bottom);
	Rect2f& Inflate(float horizontal, float vertical);
	Rect2f& Translate(const Vector2f& amount);
	Rect2f& Translate(float x, float y);
	Rect2f& Scale(float scale);
	Rect2f& Scale(const Vector2f& scale);
	Rect2f& Scale(float xScale, float yScale);

	// Static Methods.
	static Rect2f GetUnion(const Rect2f& a, const Rect2f& b);
	static Rect2f GetIntersect(const Rect2f& a, const Rect2f& b);
	static Rect2f Translate(const Rect2f& r, const Vector2f& amount);
	static Rect2f Translate(const Rect2f& r, float x, float y);
};


#endif // _RECT_2F_H_