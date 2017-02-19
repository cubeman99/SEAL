#ifndef _POINT_2I_H_
#define _POINT_2I_H_

#include <iostream>

struct Vector2f;


// 2D integer vector.
struct Point2i
{
	int x, y;
	

	// Constants.
	static const Point2i ZERO;	// (0, 0)
	static const Point2i ONE;	// (1, 1)
	static const Point2i UNITX;	// +X
	static const Point2i UNITY;	// +Y

	// Constructors.
	Point2i();
	Point2i(int x, int y);
	explicit Point2i(const Vector2f& v);
	explicit Point2i(int value);

	// Accessors.
	int			operator [](int index) const;
	int&		operator [](int index);

	// Mutators.
	Point2i&	SetZero();
	Point2i&	Set(int x, int y);
	Point2i&	Fill(int value);
	Point2i&	Negate();

	// Unary operators.
	Point2i		operator -() const;
	void		operator +=(const Point2i& v);
	void		operator -=(const Point2i& v);
	void		operator *=(const Point2i& v);
	void		operator /=(const Point2i& v);
	void		operator *=(int scalar);
	void		operator /=(int denominator);

	// Binary operators.
	Point2i		operator +(const Point2i& other) const;
	Point2i		operator -(const Point2i& other) const;
	Point2i		operator *(const Point2i& other) const;
	Point2i		operator /(const Point2i& other) const;
	Point2i		operator *(int scalar) const;
	Point2i		operator /(int denominator) const;
};


// Left-hand Operators.
Point2i			operator *(int scalar, const Point2i& v);
Point2i			operator /(int numerator, const Point2i& v);
Vector2f		operator *(float scalar, const Point2i& v);
Vector2f		operator /(float numerator, const Point2i& v);
std::ostream&	operator <<(std::ostream &out, const Point2i& v);


typedef Point2i Vector2i;


#endif // _POINT_2I_H_