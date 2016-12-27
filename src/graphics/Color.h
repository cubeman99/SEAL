#ifndef _COLOR_H_
#define _COLOR_H_

#include <math/Vector3f.h>
#include <math/Vector4f.h>


#pragma pack(1)

// RGBA8, unsigned-byte color.
struct Color
{
	unsigned char r, g, b, a;

	// Constants.
	//static const Color TRANSPARENT; // TRANSPARENT is already defined somewhere.
	static const Color WHITE;
	static const Color BLACK;
	static const Color LIGHT_GRAY;
	static const Color GRAY;
	static const Color RED;
	static const Color YELLOW;
	static const Color GREEN;
	static const Color CYAN;
	static const Color BLUE;
	static const Color MAGENTA;
	static const Color DARK_RED;
	static const Color DARK_YELLOW;
	static const Color DARK_GREEN;
	static const Color DARK_CYAN;
	static const Color DARK_BLUE;
	static const Color DARK_MAGENTA;

	// Constructors.
	Color();
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	Color(const Vector3f& v);
	Color(const Vector4f& v);

	// Accessors
	unsigned char	operator [](int index) const;
	unsigned char&	operator [](int index);
	Vector3f		ToVector3f() const;
	Vector4f		ToVector4f() const;
	Color			Inverse() const;

	unsigned char*			data();
	const unsigned char*	data() const;

	// Mutators.
	Color& Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	Color& Set(const Vector3f& v);
	Color& Set(const Vector4f& v);
	Color& SetFloat(float r, float g, float b, float a = 1.0f);
	Color& SetRGB(unsigned char r, unsigned char g, unsigned char b);
	Color& SetRGB(const Vector3f& v);
	Color& SetFloatRGB(float r, float g, float b);
	Color& Invert();

	// Unary operators.
	Color&	operator +=(const Color& other);
	Color&	operator -=(const Color& other);
	Color&	operator *=(const Color& other);
	Color&	operator *=(float scalar);

	// Binary operators
	Color	operator +(const Color& other) const;
	Color	operator -(const Color& other) const;
	Color	operator *(const Color& other) const;
	Color	operator *(float scalar) const;

	// Static Methods.
	static Color Lerp(const Color& a, const Color& b, float lerpFactor);
	static Color CreateShade(int gray, int alpha = 255);
};


// Left-hand operators.
Color operator *(float scalar, const Color& c);
std::ostream& operator <<(std::ostream &out, const Color& c);


#pragma pack()

#endif // _COLOR_H_