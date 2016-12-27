#include "Color.h"
#include <math/MathLib.h>


// Convert a color component from an unsigned byte to a float.
static float ColorUint8ToFloat(unsigned char x)
{
	return (float(x) / 255.0f);
}

// Convert a color component from a float to an unsigned byte.
static unsigned char ColorFloatToUint8(float x)
{
	if (x <= 0.0f)
		return 0;
	if (x >= 1.0f)
		return 255;
	return (unsigned char) ((x * 255.0f));
}




//-----------------------------------------------------------------------------
// Internal color componet methods
//-----------------------------------------------------------------------------

// Return the clamped sum of two color components.
static unsigned char colorSum(unsigned char a, unsigned char b)
{
	return Math::Min((int) (a + b), 255);
}

// Return the clamped difference of two color components.
static unsigned char colorDiff(unsigned char a, unsigned char b)
{
	return Math::Max((int) (a - b), 255);
}

// Return the product of two color components as normalized floats.
static unsigned char colorProduct(unsigned char a, unsigned char b)
{
	return (unsigned char) (((a / 255.0f) * (b / 255.0f)) * 255);
}

// Return the product of two color components as normalized floats.
static unsigned char colorScalarProduct(unsigned char a, float scalar)
{
	return (unsigned char) (((a / 255.0f) * scalar) * 255);
}


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

//const Color Color::TRANSPARENT	= Color(0, 0, 0, 0);
const Color Color::WHITE		= Color(255, 255, 255);
const Color Color::BLACK		= Color(  0,   0,   0);
const Color Color::LIGHT_GRAY	= Color(192, 192, 192);
const Color Color::GRAY			= Color(128, 128, 128);
const Color Color::RED			= Color(255,   0,   0);
const Color Color::YELLOW		= Color(255, 255,   0);
const Color Color::GREEN		= Color(  0, 255,   0);
const Color Color::CYAN			= Color(  0, 255, 255);
const Color Color::BLUE			= Color(  0,   0, 255);
const Color Color::MAGENTA		= Color(255,   0, 255);
const Color Color::DARK_RED		= Color(128,   0,   0);
const Color Color::DARK_YELLOW	= Color(128, 128,   0);
const Color Color::DARK_GREEN	= Color(  0, 128,   0);
const Color Color::DARK_CYAN	= Color(  0, 128, 128);
const Color Color::DARK_BLUE	= Color(  0,   0, 128);
const Color Color::DARK_MAGENTA	= Color(128,   0, 128);


//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a color with unititialized components.
Color::Color()
{
}

// Construct a color from the given RGBA components.
Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
	r(r), g(g), b(b), a(a)
{
}

// Construct a color with RGB compontents from a vector's XYZ components and an alpha of 1.
Color::Color(const Vector3f& v) :
	r(ColorFloatToUint8(v.x)),
	g(ColorFloatToUint8(v.y)),
	b(ColorFloatToUint8(v.z)),
	a(255)
{
}

// Construct a color with RGBA compontents from a vector's XYZW components.
Color::Color(const Vector4f& v) :
	r(ColorFloatToUint8(v.x)),
	g(ColorFloatToUint8(v.y)),
	b(ColorFloatToUint8(v.z)),
	a(ColorFloatToUint8(v.w))
{
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

// Access a component of this color.
unsigned char Color::operator [](int index) const
{
	//CMG_ASSERT(index >= 0 && index < 4, "Invalid component index.");
	return (&r)[index];
}

// Modify a component of this color.
unsigned char& Color::operator [](int index)
{
	//CMG_ASSERT(index >= 0 && index < 4, "Invalid component index.");
	return (&r)[index];
}

// Return this color's normalized RGB components as 3D vector.
Vector3f Color::ToVector3f() const
{
	return Vector3f(
		ColorUint8ToFloat(r),
		ColorUint8ToFloat(g),
		ColorUint8ToFloat(b)
	);
}

// Return this color's normalized RGBA components as 4D vector.
Vector4f Color::ToVector4f() const
{
	return Vector4f(
		ColorUint8ToFloat(r),
		ColorUint8ToFloat(g),
		ColorUint8ToFloat(b),
		ColorUint8ToFloat(a)
	);
}

// Return the inverse of this color.
Color Color::Inverse() const
{
	return Color(255 - r, 255 - g, 255 - b, a);
}

unsigned char* Color::data()
{
	return &r;
}

const unsigned char* Color::data() const
{
	return &r;
}



//-----------------------------------------------------------------------------
// Mutators
//-----------------------------------------------------------------------------

// Set the components.
Color& Color::Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
	return *this;
}

// Set the components from a 3D vector (setting alpha to 255)
Color& Color::Set(const Vector3f& v)
{
	r = ColorFloatToUint8(v.x);
	g = ColorFloatToUint8(v.y);
	b = ColorFloatToUint8(v.z);
	a = 255;
	return *this;
}

// Set the components from a 4D vector.
Color& Color::Set(const Vector4f& v)
{
	r = ColorFloatToUint8(v.x);
	g = ColorFloatToUint8(v.y);
	b = ColorFloatToUint8(v.z);
	a = ColorFloatToUint8(v.w);
	return *this;
}

// Set the components from a 4D vector.
Color& Color::SetFloat(float r, float g, float b, float a)
{
	this->r = ColorFloatToUint8(r);
	this->g = ColorFloatToUint8(g);
	this->b = ColorFloatToUint8(b);
	this->a = ColorFloatToUint8(a);
	return *this;
}

// Set the RGB components.
Color& Color::SetRGB(unsigned char r, unsigned char g, unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;
	return *this;
}

// Set the RGB components from a 3D vector.
Color& Color::SetRGB(const Vector3f& v)
{
	r = ColorFloatToUint8(v.x);
	g = ColorFloatToUint8(v.y);
	b = ColorFloatToUint8(v.z);
	return *this;
}

// Set the RGB components from a 3D vector.
Color& Color::SetFloatRGB(float r, float g, float b)
{
	this->r = ColorFloatToUint8(r);
	this->g = ColorFloatToUint8(g);
	this->b = ColorFloatToUint8(b);
	return *this;
}

// Invert the RGB components of this color.
Color& Color::Invert()
{
	r = 255 - r;
	g = 255 - g;
	b = 255 - b;
	return *this;
}


//-----------------------------------------------------------------------------
// Unary operators
//-----------------------------------------------------------------------------

// Add the components of another color.
Color& Color::operator +=(const Color& other)
{
	r = colorSum(r, other.r);
	g = colorSum(g, other.g);
	b = colorSum(b, other.b);
	a = colorSum(a, other.a);
	return *this;
}

// Subtract the components of another color.
Color& Color::operator -=(const Color& other)
{
	r = colorDiff(r, other.r);
	g = colorDiff(g, other.g);
	b = colorDiff(b, other.b);
	a = colorDiff(a, other.a);
	return *this;
}

// Multiply the components of another color.
Color& Color::operator *=(const Color& other)
{
	r = colorProduct(r, other.r);
	g = colorProduct(g, other.g);
	b = colorProduct(b, other.b);
	a = colorProduct(a, other.a);
	return *this;
}

// Multiply the components of this color by a scalar.
Color& Color::operator *=(float scalar)
{
	r = colorScalarProduct(r, scalar);
	g = colorScalarProduct(g, scalar);
	b = colorScalarProduct(b, scalar);
	a = colorScalarProduct(a, scalar);
	return *this;
}


//-----------------------------------------------------------------------------
// Binary operators
//-----------------------------------------------------------------------------

// Return the sum of two colors.
Color Color::operator +(const Color& other) const
{
	return Color(colorSum(r, other.r),
					colorSum(g, other.g),
					colorSum(b, other.b),
					colorSum(a, other.a));
}

// Return the difference of two colors.
Color Color::operator -(const Color& other) const
{
	return Color(colorDiff(r, other.r),
					colorDiff(g, other.g),
					colorDiff(b, other.b),
					colorDiff(a, other.a));
}

// Return the product of two colors.
Color Color::operator *(const Color& other) const
{
	return Color(colorProduct(r, other.r),
					colorProduct(g, other.g),
					colorProduct(b, other.b),
					colorProduct(a, other.a));
}

// Return a colors times a scalar.
Color Color::operator *(float scalar) const
{
	return Color(colorScalarProduct(r, scalar),
					colorScalarProduct(g, scalar),
					colorScalarProduct(b, scalar),
					colorScalarProduct(a, scalar));
}

// Return a colors times a scalar.
Color operator *(float scalar, const Color& color)
{
	return Color(colorScalarProduct(color.r, scalar),
					colorScalarProduct(color.g, scalar),
					colorScalarProduct(color.b, scalar),
					colorScalarProduct(color.a, scalar));
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Color& c)
{
	out << "(R=" << (unsigned int) c.r << ", G" << (unsigned int) c.g
		<< ", B=" << (unsigned int) c.b << ", A=" << (unsigned int) c.a << ")";
	return out;
}


//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

// Computes the linear interpolation between two colors.
Color Color::Lerp(const Color& a, const Color& b, float lerpFactor)
{
	Color result;
	return (a * (1.0f - lerpFactor)) + (b * lerpFactor);
}

// Create a color as a shade of gray, with each rgb component
// equal to the given amount, with the specified alpha
Color Color::CreateShade(int gray, int alpha)
{
	return Color(gray, gray, gray, alpha);
}


