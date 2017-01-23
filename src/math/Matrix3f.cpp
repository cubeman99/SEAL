#include "Matrix3f.h"
#include "Quaternion.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "MathLib.h"


static inline void Swap(float& a, float& b)
{
	float tmp = a;
	a = b;
	b = tmp;
}


//-----------------------------------------------------------------------------
// Matrix constants.
//-----------------------------------------------------------------------------

// The 3x3 identity matrix.
const Matrix3f Matrix3f::IDENTITY (1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f);



//-----------------------------------------------------------------------------
// Matrix constructors.
//-----------------------------------------------------------------------------

// Construct a 3x3 matrix with uninitialized components.
Matrix3f::Matrix3f()
{
}

// Construct a 3x3 matrix from data stored in column-major order.
Matrix3f::Matrix3f(const float* data)
{
	memcpy(m, data, 9 * sizeof(float));
}

// Construct a 3x3 matrix with the given components listed in row-major order.
Matrix3f::Matrix3f(	float x1, float y1, float z1,
					float x2, float y2, float z2,
					float x3, float y3, float z3)
{
	m[0] = x1; m[3] = y1; m[6] = z1;
	m[1] = x2; m[4] = y2; m[7] = z2;
	m[2] = x3; m[5] = y3; m[8] = z3;
}

// Construct a 3x3 matrix from 3 column vectors.
Matrix3f::Matrix3f(const Vector3f& xColumn, const Vector3f& yColumn, const Vector3f& zColumn)
{
	m[0] = xColumn.x; m[3] = yColumn.x; m[6] = zColumn.x;
	m[1] = xColumn.y; m[4] = yColumn.y; m[7] = zColumn.y;
	m[2] = xColumn.z; m[5] = yColumn.z; m[8] = zColumn.z;
}


//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return a pointer to a row of components.
float& Matrix3f::operator[](int index)
{
	assert(index >= 0 && index < 9);
	return m[index];
}

// Return a pointer to a row of components.
float Matrix3f::operator[](int index) const
{
	assert(index >= 0 && index < 9);
	return m[index];
}

// Return a pointer to the first component in memory.
float* Matrix3f::data()
{
	return m;
}

// Return a pointer to the first component in memory.
const float* Matrix3f::data() const
{
	return m;
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set all the components of this matrix to zero.
void Matrix3f::SetZero()
{
	memset(&m[0], 0, 9 * sizeof(float));
}

// Set this matrix to the identity matrix.
Matrix3f& Matrix3f::SetIdentity()
{
	m[0] = 1; m[3] = 0; m[6] = 0;
	m[1] = 0; m[4] = 1; m[7] = 0;
	m[2] = 0; m[5] = 0; m[8] = 1;
	return *this;
}

// Set the individual components.
void Matrix3f::Set(	float x1, float y1, float z1,
					float x2, float y2, float z2,
					float x3, float y3, float z3)
{
	m[0] = x1; m[3] = y1; m[6] = z1;
	m[1] = x2; m[4] = y2; m[7] = z2;
	m[2] = x3; m[5] = y3; m[8] = z3;
}

// Set the components of a single column from a vector.
void Matrix3f::SetColumn(int columnIndex, const Vector3f& columnVector)
{
	m[(columnIndex * 3)] = columnVector.x;
	m[(columnIndex * 3) + 1] = columnVector.y;
	m[(columnIndex * 3) + 2] = columnVector.z;
}



//-----------------------------------------------------------------------------
// Matrix operations
//-----------------------------------------------------------------------------

float Matrix3f::GetDeterminant() const
{
	return ((m[0] * m[4] * m[8]) - (m[0] * m[5] * m[7]) -
			(m[1] * m[3] * m[8]) + (m[2] * m[3] * m[7]) +
			(m[1] * m[6] * m[5]) - (m[2] * m[6] * m[4]));
}

// Transpose the matrix.
Matrix3f& Matrix3f::Transpose()
{
	Matrix3f::Transpose(*this, *this);
	return *this;
}

// Return the transpose of this matrix.
Matrix3f Matrix3f::GetTranspose() const
{
	Matrix3f result;
	Matrix3f::Transpose(*this, result);
	return result;
}

// Return the negation of this matrix.
Matrix3f Matrix3f::operator-() const
{
	Matrix3f result;
	for (int i = 0; i < 9; ++i)
		result.m[i] = -m[i];
	return result;
}

// Negate all components.
Matrix3f& Matrix3f::Negate()
{
	for (int i = 0; i < 9; ++i)
		m[i] = -m[i];
	return *this;
}

// Attempt to invert the matrix.
Matrix3f& Matrix3f::Invert()
{
	Matrix3f temp;
	Matrix3f::Invert(*this, temp);
	*this = temp;
	return *this;
}

Matrix3f Matrix3f::GetInverse() const
{
	Matrix3f inverse;
	Matrix3f::Invert(*this, inverse);
	return inverse;
}

// Scale all components.
Matrix3f Matrix3f::operator *(float scalar) const
{
	Matrix3f result;
	for (int i = 0; i < 9; ++i)
		result.m[i] = m[i] * scalar;
	return result;
}



//-----------------------------------------------------------------------------
// Matrix to Matrix operations.
//-----------------------------------------------------------------------------

// Assign the components of this matrix from another.
Matrix3f& Matrix3f::operator =(const Matrix3f& other)
{
	memcpy(m, other.m, 9 * sizeof(float));
	return *this;
}

// Add another matrix.
void Matrix3f::operator +=(const Matrix3f& other)
{
	for (int i = 0; i < 9; ++i)
		m[i] += other.m[i];
}

// Subtract another matrix.
void Matrix3f::operator -=(const Matrix3f& other)
{
	for (int i = 0; i < 9; ++i)
		m[i] -= other.m[i];
}

// Multiply by another matrix.
void Matrix3f::operator *=(const Matrix3f& other)
{
	Matrix3f temp;
	Matrix3f::Multiply(*this, other, temp);
	*this = temp;
}

// Perform a component-wise equality test.
bool Matrix3f::operator ==(const Matrix3f& other) const
{
	return (memcmp(other.m, m, sizeof(m)) == 0);
}

// Perform a component-wise inequality test.
bool Matrix3f::operator !=(const Matrix3f& other) const
{
	return (memcmp(other.m, m, sizeof(m)) != 0);
}

// Add another matrix.
Matrix3f Matrix3f::operator +(const Matrix3f& other) const
{
	Matrix3f result;
	for (int i = 0; i < 9; ++i)
		result.m[i] = m[i] + other.m[i];
	return result;
}

// Subtract another matrix.
Matrix3f Matrix3f::operator -(const Matrix3f& other) const
{
	Matrix3f result;
	for (int i = 0; i < 9; ++i)
		result.m[i] = m[i] - other.m[i];
	return result;
}

// Multiply by another matrix.
Matrix3f Matrix3f::operator *(const Matrix3f& other) const
{
	Matrix3f result;
	Matrix3f::Multiply(*this, other, result);
	return result;
}


//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

// Return the multiplication of 2 matrices.
Matrix3f Matrix3f::Multiply(const Matrix3f& left, const Matrix3f& right)
{
	Matrix3f result;
	Multiply(left, right, result);
	return result;
}

// Multiply 2 matrices.
void Matrix3f::Multiply(const Matrix3f& a, const Matrix3f& b, Matrix3f& outResult)
{
	outResult[0] = (a[0] * b[0]) + (a[3] * b[1]) + (a[6] * b[2]);
	outResult[3] = (a[0] * b[3]) + (a[3] * b[4]) + (a[6] * b[5]);
	outResult[6] = (a[0] * b[6]) + (a[3] * b[7]) + (a[6] * b[8]);
	outResult[1] = (a[1] * b[0]) + (a[4] * b[1]) + (a[7] * b[2]);
	outResult[4] = (a[1] * b[3]) + (a[4] * b[4]) + (a[7] * b[5]);
	outResult[7] = (a[1] * b[6]) + (a[4] * b[7]) + (a[7] * b[8]);
	outResult[2] = (a[2] * b[0]) + (a[5] * b[1]) + (a[8] * b[2]);
	outResult[5] = (a[2] * b[3]) + (a[5] * b[4]) + (a[8] * b[5]);
	outResult[8] = (a[2] * b[6]) + (a[5] * b[7]) + (a[8] * b[8]);
}

// Linear interpolate between 2 matrices.
void Matrix3f::Lerp(const Matrix3f& a, const Matrix3f& b, float t, Matrix3f& outResult)
{
	for (int i = 0; i < 9; ++i)
		outResult.m[i] = (a.m[i] * (1.0f - t)) + (b.m[i] * t);
}

// Return the linear interpolation between 2 matrices.
Matrix3f Matrix3f::Lerp(const Matrix3f& a, const Matrix3f& b, float t)
{
	Matrix3f result;
	Matrix3f::Lerp(a, b, t, result);
	return result;
}

// Transpose a matrix.
void Matrix3f::Transpose(const Matrix3f& src, Matrix3f& dst)
{
	if (&src == &dst)
	{
		Swap(dst[3], dst[1]);
		Swap(dst[6], dst[2]);
		Swap(dst[7], dst[5]);
	}
	else
	{
		dst[0] = src[0]; dst[3] = src[1]; dst[6] = src[2];
		dst[1] = src[3]; dst[4] = src[4]; dst[7] = src[5];
		dst[2] = src[6]; dst[5] = src[7]; dst[8] = src[8];
	}
}

// Invert a matrix.
void Matrix3f::Invert(const Matrix3f& src, Matrix3f& dst)
{
	float det = src.GetDeterminant();
	assert(det != 0.0f);
	float invDet = 1.0f / det;

	dst[0] = ((src.m[4] * src.m[8]) - (src.m[7] * src.m[5])) * invDet;
	dst[1] = ((src.m[7] * src.m[2]) - (src.m[1] * src.m[8])) * invDet;
	dst[2] = ((src.m[1] * src.m[5]) - (src.m[4] * src.m[2])) * invDet;
	dst[3] = ((src.m[6] * src.m[5]) - (src.m[3] * src.m[8])) * invDet;
	dst[4] = ((src.m[0] * src.m[8]) - (src.m[6] * src.m[2])) * invDet;
	dst[5] = ((src.m[3] * src.m[2]) - (src.m[0] * src.m[5])) * invDet;
	dst[6] = ((src.m[3] * src.m[7]) - (src.m[6] * src.m[4])) * invDet;
	dst[7] = ((src.m[6] * src.m[1]) - (src.m[0] * src.m[7])) * invDet;
	dst[8] = ((src.m[0] * src.m[4]) - (src.m[3] * src.m[1])) * invDet;
}



//-----------------------------------------------------------------------------
// Matrix to Vector operations.
//-----------------------------------------------------------------------------

// Multiply by a 2D vector (divides by z and assumes input z is 1).
void Matrix3f::TransformVector(const Vector2f& inVec, Vector2f& outVec) const
{
	float rz = 1.0f / ((m[2] * inVec.x) + (m[5] * inVec.y) + m[8]);
	outVec.x = ((m[0] * inVec.x) + (m[3] * inVec.y) + m[6]) * rz;
	outVec.y = ((m[1] * inVec.x) + (m[4] * inVec.y) + m[7]) * rz;
}

// Multiply by a 2D vector (divides by z and assumes input z is 1).
Vector2f Matrix3f::TransformVector(const Vector2f& vec) const
{
	Vector2f result;
	TransformVector(vec, result);
	return result;
}

// Multiply by a 3D column vector.
void Matrix3f::TransformVector(const Vector3f& inVec, Vector3f& outVec) const
{
	outVec.x = (m[0] * inVec.x) + (m[3] * inVec.y) + (m[6] * inVec.z);
	outVec.y = (m[1] * inVec.x) + (m[4] * inVec.y) + (m[7] * inVec.z);
	outVec.z = (m[2] * inVec.x) + (m[5] * inVec.y) + (m[8] * inVec.z);
}

// Multiply by a 3D column vector.
Vector3f Matrix3f::TransformVector(const Vector3f& vec) const
{
	Vector3f result;
	TransformVector(vec, result);
	return result;
}

// Applies a 2D rotation to a 2D vector (ignores the translation/scale components).
void Matrix3f::RotateVector(const Vector2f& inVec, Vector2f& outVec) const
{
	Multiply2x2(inVec, outVec);
	outVec.Normalize();
}

// Applies a rotation to a 2D vector (ignores the translation/scale components).
Vector2f Matrix3f::RotateVector(const Vector2f& vec) const
{
	Vector2f result;
	RotateVector(vec, result);
	return result;
}

// Applies a 3D rotation to a 3D vector.
void Matrix3f::RotateVector(const Vector3f& inVec, Vector3f& outVec) const
{
	TransformVector(inVec, outVec);
	outVec.Normalize();
}

// Applies a rotation to a 3D vector.
Vector3f Matrix3f::RotateVector(const Vector3f& vec) const
{
	Vector3f result;
	RotateVector(vec, result);
	return result;
}

// Mutliply the upper-left 2x2 sub-matrix by a 2D vector.
void Matrix3f::Multiply2x2(const Vector2f& inVec, Vector2f& outVec) const
{
	outVec.x = (m[0] * inVec.x) + (m[3] * inVec.y);
	outVec.y = (m[1] * inVec.x) + (m[4] * inVec.y);
}

// Mutliply the upper-left 2x2 sub-matrix by a 2D vector.
Vector2f Matrix3f::Multiply2x2(const Vector2f& vVec) const
{
	Vector2f result;
	Multiply2x2(vVec, result);
	return result;
}

// Multiply by a 2D column vector (divides by z and assumes input z is 1)
Vector2f Matrix3f::operator *(const Vector2f& vec) const
{
	Vector2f result;
	TransformVector(vec, result);
	return result;
}

// Multiply with a 3D column vector.
Vector3f Matrix3f::operator *(const Vector3f& vec) const
{
	Vector3f result;
	TransformVector(vec, result);
	return result;
}



//-----------------------------------------------------------------------------
// Matrix Creation
//-----------------------------------------------------------------------------

Matrix3f& Matrix3f::InitTranslation(float x, float y)
{
	m[0] = 1; m[3] = 0; m[6] = x;
	m[1] = 0; m[4] = 1; m[7] = y;
	m[2] = 0; m[5] = 0; m[8] = 1;
	return *this;
}

Matrix3f& Matrix3f::InitTranslation(const Vector2f& translation)
{
	m[0] = 1; m[3] = 0; m[6] = translation.x;
	m[1] = 0; m[4] = 1; m[7] = translation.y;
	m[2] = 0; m[5] = 0; m[8] = 1;
	return *this;
}

Matrix3f& Matrix3f::InitScale(float scale)
{
	return InitScale(scale, scale, scale);
}

Matrix3f& Matrix3f::InitScale(float x, float y)
{
	m[0] = x; m[3] = 0; m[6] = 0;
	m[1] = 0; m[4] = y; m[7] = 0;
	m[2] = 0; m[5] = 0; m[8] = 1;
	return *this;
}

Matrix3f& Matrix3f::InitScale(float x, float y, float z)
{
	m[0] = x; m[3] = 0; m[6] = 0;
	m[1] = 0; m[4] = y; m[7] = 0;
	m[2] = 0; m[5] = 0; m[8] = z;
	return *this;
}

Matrix3f& Matrix3f::InitScale(const Vector2f& scale)
{
	return InitScale(scale.x, scale.y, 1.0f);
}

Matrix3f& Matrix3f::InitScale(const Vector3f& scale)
{
	return InitScale(scale.x, scale.y, scale.z);
}

Matrix3f& Matrix3f::InitRotation(float angle)
{
	float cosAngle = Math::Cos(angle);
	float sinAngle = Math::Sin(angle);
	m[0] =  cosAngle; m[3] = sinAngle; m[6] = 0.0f;
	m[1] = -sinAngle; m[4] = cosAngle; m[7] = 0.0f;
	m[2] = 0.0f;      m[5] = 0.0f;     m[8] = 1.0f;
	return *this;
}

Matrix3f& Matrix3f::InitRotation(const Vector2f& vOrigin, float angle)
{
	// 1. Translate by negated origin.
	// 2. Rotate by angle.
	// 3. Translate by origin.
	*this = Matrix3f::CreateTranslation(vOrigin) *	// 3.
			Matrix3f::CreateRotation(angle) *		// 2.
			Matrix3f::CreateTranslation(-vOrigin);	// 1.
	return *this;
}

Matrix3f& Matrix3f::InitRotation(const Vector3f& axis, float angle)
{
	return InitRotation(Quaternion(axis, angle));
}

Matrix3f& Matrix3f::InitRotation(const Quaternion& q)
{
	m[0] = 1 - ((2 * q.y*q.y) + (2 * q.z*q.z));
	m[1] = (2 * q.x*q.y) - (2 * q.z*q.w);
	m[2] = (2 * q.x*q.z) + (2 * q.y*q.w);
	m[3] = (2 * q.x*q.y) + (2 * q.z*q.w);
	m[4] = 1 - ((2 * q.x*q.x) + (2 * q.z*q.z));
	m[5] = (2 * q.y*q.z) - (2 * q.x*q.w);
	m[6]  = (2 * q.x*q.z) - (2 * q.y*q.w);
	m[7]  = (2 * q.y*q.z) + (2 * q.x*q.w);
	m[8] = 1 - ((2 * q.x*q.x) + (2 * q.y*q.y));
	return *this;
}

Matrix3f Matrix3f::CreateIdentity()
{
	Matrix3f result;
	result.SetIdentity();
	return result;
}

Matrix3f Matrix3f::CreateTranslation(float x, float y)
{
	Matrix3f result;
	result.InitTranslation(x, y);
	return result;
}

Matrix3f Matrix3f::CreateTranslation(const Vector2f& translation)
{
	Matrix3f result;
	result.InitTranslation(translation);
	return result;
}

Matrix3f Matrix3f::CreateScale(float scale)
{
	Matrix3f result;
	result.InitScale(scale, scale, scale);
	return result;
}

Matrix3f Matrix3f::CreateScale(float x, float y)
{
	Matrix3f result;
	result.InitScale(x, y);
	return result;
}

Matrix3f Matrix3f::CreateScale(float x, float y, float z)
{
	Matrix3f result;
	result.InitScale(x, y, z);
	return result;
}

Matrix3f Matrix3f::CreateScale(const Vector2f& scale)
{
	Matrix3f result;
	result.InitScale(scale);
	return result;
}

Matrix3f Matrix3f::CreateScale(const Vector3f& scale)
{
	Matrix3f result;
	result.InitScale(scale);
	return result;
}

// Create a 2D rotation matrix with the given clockwise angle in radians.
Matrix3f Matrix3f::CreateRotation(float angle)
{
	Matrix3f result;
	result.InitRotation(angle);
	return result;
}

// Rotation about an 
Matrix3f Matrix3f::CreateRotation(const Vector2f& origin, float angle)
{
	Matrix3f result;
	result.InitRotation(origin, angle);
	return result;
}

Matrix3f Matrix3f::CreateRotation(const Vector3f& axis, float angle)
{
	Matrix3f result;
	result.InitRotation(axis, angle);
	return result;
}

Matrix3f Matrix3f::CreateRotation(const Quaternion& rotation)
{
	Matrix3f result;
	result.InitRotation(rotation);
	return result;
}

