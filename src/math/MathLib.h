#ifndef _MATH_LIB_H_
#define _MATH_LIB_H_

#include <cmath>
#include <cstdlib>
#include <assert.h>


// Class for static math functions.
class Math
{
public:
		
	//-----------------------------------------------------------------------------
	// Math Constants
	//-----------------------------------------------------------------------------
		
	static const float PI;			// Pi
	static const float TWO_PI;		// Two pi
	static const float HALF_PI;		// Half pi
	static const float INV_PI;		// Inverse pi
	static const float INV_TWO_PI;	// Inverse 2 pi
	static const float DEG_TO_RAD;	// Multiply a degree value by this to convert it to radians.
	static const float RAD_TO_DEG;	// Multiply a radian value by this to convert it to degrees.
	static const float LOG_2;		// Log base 10 of 2.
	static const float LOG_10;		// Log base 10 of 10.
	static const float INV_LOG_2;	// 1 / ln(2)
	static const float INV_LOG_10;	// 1 / ln(10)

	
	//-----------------------------------------------------------------------------
	// Math Functions
	//-----------------------------------------------------------------------------

	template<class T>
	static T IsNaN(T x);

	
	template<class T>
	static T Min(T a, T b);
	
	template<class T>
	static T Max(T a, T b);

	template <class T>
	static T Clamp(T value, T minValue, T maxValue);

	template <class T>
	static T Lerp(T from, T to, T x);


	template <typename T>
	static T Abs(T x);
		
	template <typename T>
	static T Sign(T x);

		
	template <typename T>
	static T Remainder(T numerator, T denominator);
		
	template <typename T>
	static T Wrap(T x, T y);
	
	template <typename T>
	static T Floor(T x);
	
	template <typename T>
	static T Ceil(T x);
	
	template <typename T>
	static int NInt(T x);

	
	template <typename T>
	static T Sqr(T x);
	
	template <typename T>
	static T Sqrt(T x);
	
	template <typename T>
	static T InvSqrt(T x);

	
	template <typename T>
	static T Log(T x);

	template <typename T>
    static T Log2(T value);

	template <typename T>
    static T Log10(T value);

	template <typename T>
    static T LogN(T base, T value);
		
	
	template <typename T>
	static T Exp(T x);

	template <typename T>
    static T Pow(T base, T exponent);

	
	template <typename T>
	static T Sin(T x);
	
	template <typename T>
	static T ASin(T x);
	
	template <typename T>
	static T Cos(T x);
	
	template <typename T>
	static T ACos(T x);
	
	template <typename T>
	static T Tan(T x);
	
	template <typename T>
	static T ATan(T x);
	
	template <typename T>
	static T ATan2(T y, T x);
	
	template <typename T>
	static T ToRadians(T degrees);
	
	template <typename T>
	static T ToDegrees(T radians);
};


	
//-----------------------------------------------------------------------------
// Math Functions
//-----------------------------------------------------------------------------
	
template<class T>
T Math::IsNaN(T x)
{
	return (x != x);
}

template<class T>
T Math::Min(T a, T b)
{
	return (a < b ? a : b);
}
	
template<class T>
T Math::Max(T a, T b)
{
	return (a > b ? a : b);
}

template <class T>
T Math::Clamp(T value, T minValue, T maxValue)
{
	if (value < minValue)
		return minValue;
	else if (value > maxValue)
		return maxValue;
	else
		return value;
}

template <class T>
T Math::Lerp(T from, T to, T x)
{
	return (from + (x * (to - from)));
}

template <typename T>
T Math::Abs(T x)
{
	return (T) std::abs(x);
}

template <typename T>
T Math::Sign(T x)
{
	return (x > T(0) ? T(1) : (x < T(0) ? T(-1) : T(0)));
}
		
template <typename T>
T Math::Remainder(T numerator, T denominator)
{
	// UNIMPLEMENTED FUNCTION!!!
	return T(0);
}

template <typename T>
T Math::Wrap(T x, T y)
{
	if (x > (T) 0)
		return Remainder(x, y);
	else
		return return Remainder(y - Remainder(-x, y), y);
}
	

template <typename T>
T Math::Floor(T x)
{
	return (T) std::floor(x);
}
	
template <typename T>
T Math::Ceil(T x)
{
	return (T) std::ceil(x);
}
	
// Nearest integer.
template <typename T>
int Math::NInt(T x)
{
	return (int) (x + (x < (T) 0 ? (T) -0.499999999 : (T) 0.499999999));
}
	
template <typename T>
T Math::Sqr(T x)
{
	return (x * x);
}
	
template <typename T>
T Math::Sqrt(T x)
{
	return (T) std::sqrt(x);
}
	
template <typename T>
T Math::InvSqrt(T x)
{
	return ((T) 1) / std::floor(x);
}
	
template <typename T>
T Math::Log(T x)
{
	return (T) std::log(x);
}

template <typename T>
T Math::Log2(T value)
{
	return (T) (std::log(value) / (T) LOG_2);
}

template <typename T>
T Math::Log10(T value)
{
	return (T) (std::log(value) / (T) LOG_10);
}

template <typename T>
T Math::LogN(T base, T value)
{
	return (T) (log(value) / log(base));
}
	
template <typename T>
T Math::Exp(T x)
{
	return (T) std::exp(x);
}

template <typename T>
T Math::Pow(T base, T exponent)
{
	return (T) std::pow(base, exponent);
}
	
template <typename T>
T Math::Sin(T x)
{
	return (T) std::sin(x);
}
	
template <typename T>
T Math::ASin(T x)
{
	return (T) std::asin(x);
}
	
template <typename T>
T Math::Cos(T x)
{
	return (T) std::cos(x);
}
	
template <typename T>
T Math::ACos(T x)
{
	return (T) std::acos(x);
}
	
template <typename T>
T Math::Tan(T x)
{
	return (T) std::tan(x);
}
	
template <typename T>
T Math::ATan(T x)
{
	return (T) std::atan(x);
}
	
template <typename T>
T Math::ATan2(T y, T x)
{
	return (T) std::atan2(y, x);
}

template <typename T>
T Math::ToRadians(T degrees)
{
	return (degrees * DEG_TO_RAD);
}

template <typename T>
T Math::ToDegrees(T radians)
{
	return (radians * RAD_TO_DEG);
}


#endif // _MATH_LIB_H_