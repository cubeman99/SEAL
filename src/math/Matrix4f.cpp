#include "Matrix4f.h"
#include "Matrix3f.h"
#include "Quaternion.h"
#include "Vector4f.h"
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

// The 4x4 identity matrix.
const Matrix4f Matrix4f::IDENTITY(	1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1);


//-----------------------------------------------------------------------------
// Matrix constructors.
//-----------------------------------------------------------------------------

// Construct a 4x4 matrix with uninitialized components.
Matrix4f::Matrix4f()
{
}

// Construct a 4x4 matrix from data stored in column-major order.
Matrix4f::Matrix4f(const float* data)
{
	memcpy(m, data, 16 * sizeof(float));
}

// Construct a 4x4 matrix from the given values listed in row-major order.
Matrix4f::Matrix4f(	float x1, float y1, float z1, float w1,
					float x2, float y2, float z2, float w2,
					float x3, float y3, float z3, float w3,
					float x4, float y4, float z4, float w4)
{
	m[0] = x1; m[4] = y1; m[8]  = z1; m[12] = w1;
	m[1] = x2; m[5] = y2; m[9]  = z2; m[13] = w2;
	m[2] = x3; m[6] = y3; m[10] = z3; m[14] = w3;
	m[3] = x4; m[7] = y4; m[11] = z4; m[15] = w4;
}

// Construct a 4x4 matrix given the basis vectors for the X, Y, and Z axes.
Matrix4f::Matrix4f(	const Vector3f& xBasis, const Vector3f& yBasis,
					const Vector3f& zBasis)
{
	m[0] = xBasis.x; m[4] = yBasis.x; m[8]  = zBasis.x; m[12] = 0;
	m[1] = xBasis.y; m[5] = yBasis.y; m[9]  = zBasis.y; m[13] = 0;
	m[2] = xBasis.z; m[6] = yBasis.z; m[10] = zBasis.z; m[14] = 0;
	m[3] = 0;        m[7] = 0;        m[11] = 0;        m[15] = 1;
}

// Construct a 4x4 matrix given the basis vectors for the X, Y,
// and Z axes, along with a translation vector.
Matrix4f::Matrix4f(	const Vector3f& xBasis, const Vector3f& yBasis,
					const Vector3f& zBasis, const Vector3f& translation)
{
	m[0] = xBasis.x; m[4] = yBasis.x; m[8]  = zBasis.x; m[12] = translation.x;
	m[1] = xBasis.y; m[5] = yBasis.y; m[9]  = zBasis.y; m[13] = translation.y;
	m[2] = xBasis.z; m[6] = yBasis.z; m[10] = zBasis.z; m[14] = translation.z;
	m[3] = 0;        m[7] = 0;        m[11] = 0;        m[15] = 1;
}

// Construct a 4x4 matrix from 4 column vectors.
Matrix4f::Matrix4f(	const Vector4f& xColumn, const Vector4f& yColumn,
					const Vector4f& zColumn, const Vector4f& wColumn)
{
	m[0] = xColumn.x; m[4] = yColumn.x; m[8]  = zColumn.x; m[12] = wColumn.x;
	m[1] = xColumn.y; m[5] = yColumn.y; m[9]  = zColumn.y; m[13] = wColumn.y;
	m[2] = xColumn.z; m[6] = yColumn.z; m[10] = zColumn.z; m[14] = wColumn.z;
	m[3] = xColumn.w; m[7] = yColumn.w; m[11] = zColumn.w; m[15] = wColumn.w;
}

// Construct a 4x4 matrix from a 3x3 matrix.
Matrix4f::Matrix4f(const Matrix3f& mat3x3)
{
	m[0] = mat3x3.m[0]; m[4] = mat3x3.m[3]; m[8]  = mat3x3.m[6]; m[12] = 0;
	m[1] = mat3x3.m[1]; m[5] = mat3x3.m[4]; m[9]  = mat3x3.m[7]; m[13] = 0;
	m[2] = mat3x3.m[2]; m[6] = mat3x3.m[5]; m[10] = mat3x3.m[8]; m[14] = 0;
	m[3] = 0;           m[7] = 0;           m[11] = 0;           m[15] = 1;
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

// Obtain a reference to an indexed component (in column-major order).
float& Matrix4f::operator [](int index)
{
	assert(index >= 0 && index < 16);
	return m[index];
}

// Access an indexed component (in column-major order).
float Matrix4f::operator [](int index) const
{
	assert(index >= 0 && index < 16);
	return m[index];
}

// Return a pointer to the first component in memory.
float* Matrix4f::data()
{
	return m;
}

// Return a pointer to the first component in memory.
const float* Matrix4f::data() const
{
	return m;
}

float Matrix4f::GetDeterminant() const
{
	return	(m[0]  * Matrix3f(	m[5], m[9],  m[13],
								m[6], m[10], m[14],
								m[7], m[11], m[15]).GetDeterminant()) -
			(m[4]  * Matrix3f(	m[1], m[9],  m[13],
								m[2], m[10], m[14],
								m[3], m[11], m[15]).GetDeterminant()) +
			(m[8]  * Matrix3f(	m[1], m[5],  m[13],
								m[2], m[6],  m[14],
								m[3], m[7],  m[15]).GetDeterminant()) -
			(m[12] * Matrix3f(	m[1], m[5],  m[9],
								m[2], m[6],  m[10],
								m[3], m[7],  m[11]).GetDeterminant());

	// NOTE: This is for a 4x3 matrix
	//return  (m[8] * m[5] * m[2]) +
	//		(m[4] * m[9] * m[2]) +
	//		(m[8] * m[1] * m[2]) -
	//		(m[0] * m[9] * m[2]) -
	//		(m[4] * m[1] * m[2]) +
	//		(m[0] * m[5] * m[2]);
}

Vector4f Matrix4f::GetColumn(int columnIndex) const
{
	return Vector4f(m[(columnIndex * 4)],
					m[(columnIndex * 4) + 1],
					m[(columnIndex * 4) + 2],
					m[(columnIndex * 4) + 3]);
}

bool Matrix4f::IsAffine() const
{
	// Affine transformations don't use the bottom row.
	return (m[3] == 0.0f && m[7] == 0.0f && m[11] == 0.0f && m[15] == 1.0f);
}

// Return the X basis vector.
Vector3f Matrix4f::GetXBasis() const
{
	return Vector3f(m[0], m[1], m[2]);
}

// Return the Y basis vector.
Vector3f Matrix4f::GetYBasis() const
{
	return Vector3f(m[4], m[5], m[6]);
}

// Return the Z basis vector.
Vector3f Matrix4f::GetZBasis() const
{
	return Vector3f(m[8], m[9], m[10]);
}

// Return the translation vector.
Vector3f Matrix4f::GetTranslation() const
{
	return Vector3f(m[12], m[13], m[14]);
}

// Get the pure rotation as a quaternion from the 3x3 submatrix
// (if it is orthogonal).
Quaternion Matrix4f::GetRotation() const
{
	Quaternion out;
	float trace = m[0] + m[5] + m[10];
		
	if (trace > 0.0f)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		out.w = 0.25f / s;
		out.x = (m[9] - m[6]) * s;
		out.y = (m[2] - m[8]) * s;
		out.z = (m[1] - m[4]) * s;
	}
	else if (m[0] > m[5] && m[0] > m[10])
	{
		float s = 2.0f * sqrtf(1.0f + m[0] - m[5] - m[10]);
		out.w = (m[9] - m[6]) / s;
		out.x = 0.25f * s;
		out.y = (m[1] + m[4]) / s;
		out.z = (m[2] + m[8]) / s;
	}
	else if (m[5] > m[10])
	{
		float s = 2.0f * sqrtf(1.0f + m[5] - m[0] - m[10]);
		out.w = (m[2] - m[8]) / s;
		out.x = (m[1] + m[4]) / s;
		out.y = 0.25f * s;
		out.z = (m[6] + m[9]) / s;
	}
	else
	{
		float s = 2.0f * sqrtf(1.0f + m[10] - m[5] - m[0]);
		out.w = (m[4] - m[1]) / s;
		out.x = (m[2] + m[8]) / s;
		out.y = (m[9] + m[6]) / s;
		out.z = 0.25f * s;
	}

	out.Normalize();
	return out;
}

// Return the upper-left 3x3 sub-matrix.
Matrix3f Matrix4f::Get3x3() const
{
	return Matrix3f(m[0], m[4], m[8],
					m[1], m[5], m[9],
					m[2], m[6], m[10]);
}


//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set all the components of this matrix to zero.
Matrix4f& Matrix4f::SetZero()
{
	memset(m, 0, 16 * sizeof(float));
	return *this;
}

// Set this matrix to the identity matrix.
Matrix4f& Matrix4f::SetIdentity()
{
	m[0] = 1; m[4] = 0; m[8]  = 0; m[12] = 0;
	m[1] = 0; m[5] = 1; m[9]  = 0; m[13] = 0;
	m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
	m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
	return *this;
}

// Set the individual components, listed in row-major order.
void Matrix4f::Set(	float x1, float y1, float z1, float w1,
					float x2, float y2, float z2, float w2,
					float x3, float y3, float z3, float w3,
					float x4, float y4, float z4, float w4)
{
	m[0] = x1; m[4] = y1; m[8]  = z1; m[12] = w1;
	m[1] = x2; m[5] = y2; m[9]  = z2; m[13] = w2;
	m[2] = x3; m[6] = y3; m[10] = z3; m[14] = w3;
	m[3] = x4; m[7] = y4; m[11] = z4; m[15] = w4;
}

// Set the components of a single column from a vector.
void Matrix4f::SetColumn(int columnIndex, const Vector4f& columnVector)
{
	m[(columnIndex * 4)] = columnVector.x;
	m[(columnIndex * 4) + 1] = columnVector.y;
	m[(columnIndex * 4) + 2] = columnVector.z;
	m[(columnIndex * 4) + 3] = columnVector.w;
}

// Set the X basis vector.
void Matrix4f::SetXBasis(const Vector3f& xBasis)
{
	m[0] = xBasis.x;
	m[1] = xBasis.y;
	m[2] = xBasis.z;
}

// Set the Y basis vector.
void Matrix4f::SetYBasis(const Vector3f& yBasis)
{
	m[4] = yBasis.x;
	m[5] = yBasis.y;
	m[6] = yBasis.z;
}

// Set the Z basis vector.
void Matrix4f::SetZBasis(const Vector3f& zBasis)
{
	m[8] = zBasis.x;
	m[9] = zBasis.y;
	m[10] = zBasis.z;
}

// Set the translation column of the matrix transformation.
void Matrix4f::SetTranslation(const Vector3f& translation)
{
	m[12] = translation.x;
	m[13] = translation.y;
	m[14] = translation.z;
}

// Set the three basis vectors (x, y, and z).
void Matrix4f::SetBasisVectors(	const Vector3f& xBasis,
								const Vector3f& yBasis,
								const Vector3f& zBasis)
{
	m[0] = xBasis.x; m[4] = yBasis.x; m[8]  = zBasis.x;
	m[1] = xBasis.y; m[5] = yBasis.y; m[9]  = zBasis.y;
	m[2] = xBasis.z; m[6] = yBasis.z; m[10] = zBasis.z;
}

// Set the values of the upper-left 3x3 sub-matrix.
void Matrix4f::Set3x3(const Matrix3f& mat3x3)
{
	m[0] = mat3x3.m[0]; m[4] = mat3x3.m[3]; m[8]  = mat3x3.m[6];
	m[1] = mat3x3.m[1]; m[5] = mat3x3.m[4]; m[9]  = mat3x3.m[7];
	m[2] = mat3x3.m[2]; m[6] = mat3x3.m[5]; m[10] = mat3x3.m[8];
}


//-----------------------------------------------------------------------------
// Matrix operations
//-----------------------------------------------------------------------------

// Transpose the matrix.
Matrix4f& Matrix4f::Transpose()
{
	Swap(m[1], m[4]);
	Swap(m[2], m[8]);
	Swap(m[3], m[12]);
	Swap(m[6], m[9]);
	Swap(m[7], m[13]);
	Swap(m[11], m[14]);
	return *this;
}

// Return the transpose of this matrix.
Matrix4f Matrix4f::GetTranspose() const
{
	Matrix4f result;
	Matrix4f::Transpose(*this, result);
	return result;
}

// Negate all components.
Matrix4f& Matrix4f::Negate()
{
	for (int i = 0; i < 16; ++i)
		m[i] = -m[i];
	return *this;
}

// Return the negation of this matrix.
Matrix4f Matrix4f::operator-() const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.m[i] = -m[i];
	return result;
}

Matrix4f& Matrix4f::Invert()
{
	Matrix4f temp;
	Matrix4f::Invert(*this, temp);
	*this = temp;
	return *this;
}

Matrix4f Matrix4f::GetInverse() const
{
	Matrix4f inverse;
	Matrix4f::Invert(*this, inverse);
	return inverse;
}

Matrix4f& Matrix4f::InvertAffine()
{
	Matrix4f temp;
	Matrix4f::InvertAffine(*this, temp);
	*this = temp;
	return *this;
}

Matrix4f Matrix4f::GetAffineInverse() const
{
	Matrix4f inverse;
	Matrix4f::InvertAffine(*this, inverse);
	return inverse;
}


//-----------------------------------------------------------------------------
// Matrix to Matrix operations
//-----------------------------------------------------------------------------

// Assign the components of this matrix from another.
Matrix4f& Matrix4f::operator =(const Matrix4f& other)
{
	memcpy(m, other.m, 16 * sizeof(float));
	return *this;
}

// Add another matrix.
void Matrix4f::operator +=(const Matrix4f& other)
{
	for (int i = 0; i < 16; ++i)
		m[i] += other.m[i];
}

// Subtract another matrix.
void Matrix4f::operator -=(const Matrix4f& other)
{
	for (int i = 0; i < 16; ++i)
		m[i] -= other.m[i];
}

// Multiply by another matrix.
void Matrix4f::operator *=(const Matrix4f& other)
{
	Matrix4f temp;
	Matrix4f::Multiply(*this, other, temp);
	*this = temp;
}

// Perform a component-wise equality test.
bool Matrix4f::operator ==(const Matrix4f& other) const
{
	return (memcmp(m, other.m, 16 * sizeof(float)) == 0);
}

// Perform a component-wise inequality test.
bool Matrix4f::operator !=(const Matrix4f& other) const
{
	return (memcmp(m, other.m, 16 * sizeof(float)) != 0);
}

// Component-wise addition.
Matrix4f Matrix4f::operator +(const Matrix4f& other) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.m[i] = m[i] + other.m[i];
	return result;
}

// Component-wise subtraction.
Matrix4f Matrix4f::operator -(const Matrix4f& other) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.m[i] = m[i] - other.m[i];
	return result;
}

// Multiply by another matrix.
Matrix4f Matrix4f::operator *(const Matrix4f& other) const
{
	Matrix4f result;
	Matrix4f::Multiply(*this, other, result);
	return result;
}

// Scale all components.
Matrix4f Matrix4f::operator*(float scalar) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.m[i] = m[i] * scalar;
	return result;
}


//-----------------------------------------------------------------------------
// Matrix to Vector operations
//-----------------------------------------------------------------------------

// Mutliply the upper-left 3x3 sub-matrix by a 3D vector (this
// will effectively apply a rotation).
void Matrix4f::Multiply3x3(const Vector3f& inVec, Vector3f& outVec) const
{
	outVec.x = (m[0] * inVec.x) + (m[4] * inVec.y) + (m[8]  * inVec.z);
	outVec.y = (m[1] * inVec.x) + (m[5] * inVec.y) + (m[9]  * inVec.z);
	outVec.z = (m[2] * inVec.x) + (m[6] * inVec.y) + (m[10] * inVec.z);
}

// Mutliply the upper 3 rows by a 3D vector.
void Matrix4f::Multiply4x3(const Vector3f& inVec, Vector3f& outVec) const
{
	outVec.x = (m[0] * inVec.x) + (m[4] * inVec.y) + (m[8]  * inVec.z) + m[12];
	outVec.y = (m[1] * inVec.x) + (m[5] * inVec.y) + (m[9]  * inVec.z) + m[13];
	outVec.z = (m[2] * inVec.x) + (m[6] * inVec.y) + (m[10] * inVec.z) + m[14];
}

// Mutliply the upper-left 3x3 sub-matrix by a 3D vector (this will
// effectively apply a rotation and/or scale).
Vector3f Matrix4f::Multiply3x3(const Vector3f& vec) const
{
	Vector3f result;
	Multiply3x3(vec, result);
	return result;
}

// Mutliply the upper 3 rows by a 3D vector.
Vector3f Matrix4f::Multiply4x3(const Vector3f& vVec) const
{
	Vector3f result;
	Multiply4x3(vVec, result);
	return result;
}

// Multiply by a 3D vector (divides by w and assumes input w is 1)
void Matrix4f::ApplyTransform(const Vector3f& inVec, Vector3f& outVec) const
{
	float rw = 1.0f / ((m[3] * inVec.x) + (m[7] * inVec.y) + (m[11] * inVec.z) + m[15]);
	outVec.x = ((m[0] * inVec.x) + (m[4] * inVec.y) + (m[8]  * inVec.z) + m[12]) * rw;
	outVec.y = ((m[1] * inVec.x) + (m[5] * inVec.y) + (m[9]  * inVec.z) + m[13]) * rw;
	outVec.z = ((m[2] * inVec.x) + (m[6] * inVec.y) + (m[10] * inVec.z) + m[14]) * rw;
}

// Applies a rotation to a 3D vector (ignores the translation
// components, this just calls Multiply3x3)
void Matrix4f::ApplyRotation(const Vector3f& inVec, Vector3f& outVec) const
{
	Multiply3x3(inVec, outVec);
	outVec.Normalize();
}

// Multiply by a 4D vector.
void Matrix4f::ApplyTransform(const Vector4f& inVec, Vector4f& outVec) const
{
	outVec[0] = (m[0] * inVec.x) + (m[4] * inVec.y) + (m[8]  * inVec.z) + (m[12] * inVec.w);
	outVec[1] = (m[1] * inVec.x) + (m[5] * inVec.y) + (m[9]  * inVec.z) + (m[13] * inVec.w);
	outVec[2] = (m[2] * inVec.x) + (m[6] * inVec.y) + (m[10] * inVec.z) + (m[14] * inVec.w);
	outVec[3] = (m[3] * inVec.x) + (m[7] * inVec.y) + (m[11] * inVec.z) + (m[15] * inVec.w);
}

// Multiply by a 3D vector (divides by w and assumes input w is 1)
Vector3f Matrix4f::ApplyTransform(const Vector3f& vec) const
{
	Vector3f result;
	ApplyTransform(vec, result);
	return result;
}

// Applies a rotation to a 3D vector (ignores the translation
// components, this just calls Multiply3x3)
Vector3f Matrix4f::ApplyRotation(const Vector3f& vec) const
{
	Vector3f result;
	ApplyRotation(vec, result);
	return result;
}

// Multiply by a 4D vector.
Vector4f Matrix4f::ApplyTransform(const Vector4f& vec) const
{
	Vector4f result;
	ApplyTransform(vec, result);
	return result;
}

// Multiply by a 3D vector (divides by w and assumes input w is 1)
Vector3f Matrix4f::operator*(const Vector3f& vec) const
{
	Vector3f result;
	ApplyTransform(vec, result);
	return result;
}

// Multiply by a 4D vector.
Vector4f Matrix4f::operator*(const Vector4f& vec) const
{
	Vector4f result;
	ApplyTransform(vec, result);
	return result;
}



//-----------------------------------------------------------------------------
// Matrix initialization
//-----------------------------------------------------------------------------

Matrix4f& Matrix4f::InitTranslation(float x, float y, float z)
{
	m[0] = 1; m[4] = 0; m[8]  = 0; m[12] = x;
	m[1] = 0; m[5] = 1; m[9]  = 0; m[13] = y;
	m[2] = 0; m[6] = 0; m[10] = 1; m[14] = z;
	m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
	return *this;
}

Matrix4f& Matrix4f::InitTranslation(const Vector3f& translation)
{
	return InitTranslation(translation.x, translation.y, translation.z);
}

Matrix4f& Matrix4f::InitScale(float scalar)
{
	return InitScale(scalar, scalar, scalar);
}

Matrix4f& Matrix4f::InitScale(float x, float y, float z)
{
	m[0] = x; m[4] = 0; m[8]  = 0; m[12] = 0;
	m[1] = 0; m[5] = y; m[9]  = 0; m[13] = 0;
	m[2] = 0; m[6] = 0; m[10] = z; m[14] = 0;
	m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
	return *this;
}

Matrix4f& Matrix4f::InitScale(const Vector3f& scale)
{
	return InitScale(scale.x, scale.y, scale.z);
}

Matrix4f& Matrix4f::InitRotation(const Vector3f& axis, float angle)
{
	return InitRotation(Quaternion(axis, angle));
}

Matrix4f& Matrix4f::InitRotation(const Quaternion& q)
{
	// Simplified form of rotating each basis vector
	
	// Rotate the unit-x vector by the quaternion.
	m[0]  = ((q.w * q.w) + (q.x * q.x) - (q.z * q.z) - (q.y * q.y));
	m[1]  = 2 * ((q.x * q.y) + (q.z * q.w));
	m[2]  = 2 * ((q.z * q.x) - (q.y * q.w));
	m[3]  = 0;
	
	// Rotate the unit-y vector by the quaternion.
	m[4]  = 2 * ((q.x * q.y) - (q.z * q.w));
	m[5]  = ((q.w * q.w) + (q.y * q.y) - (q.z * q.z) - (q.x * q.x));
	m[6]  = 2 * ((q.y * q.z) + (q.x * q.w));
	m[7]  = 0;
	
	// Rotate the unit-z vector by the quaternion.
	m[8]  = 2 * ((q.z * q.x) + (q.y * q.w));
	m[9]  = 2 * ((q.y * q.z) - (q.x * q.w));
	m[10] = ((q.w * q.w) + (q.z * q.z) - (q.x * q.x) - (q.y * q.y));
	m[11] = 0;

	// Set the bottom row to the identity.
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;

	return *this;
}

Matrix4f& Matrix4f::InitLookAt(const Vector3f& lookAtDir, const Vector3f& upDir)
{
	// Orthonormalize into three basis vectors
	SetIdentity();
	Vector3f zBasis = -lookAtDir;
	zBasis.Normalize();
	Vector3f xBasis = upDir.Cross(zBasis);
	xBasis.Normalize();
	Vector3f yBasis = zBasis.Cross(xBasis);
	yBasis.Normalize();
	SetXBasis(xBasis);
	SetYBasis(yBasis);
	SetZBasis(zBasis);
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveX(float fovX, float aspect,
									float zNear, float zFar)
{
	float invTanHalfFoV = 1.0f / Math::Tan(fovX * 0.5f);
	DoInitPerspective(
		invTanHalfFoV,
		invTanHalfFoV * aspect,
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveY(float fovY, float aspect,
									float zNear, float zFar)
{
	float invTanHalfFoV = 1.0f / Math::Tan(fovY * 0.5f);
	DoInitPerspective(
		invTanHalfFoV / aspect,
		invTanHalfFoV,
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveXY(float fovX, float fovY,
									float zNear, float zFar)
{
	DoInitPerspective(
		1.0f / Math::Tan(fovX * 0.5f),
		1.0f / Math::Tan(fovY * 0.5f),
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitOrthographic(float left, float right,
									float bottom, float top,
									float zNear, float zFar)
{
	SetIdentity();

	float invWidth  = 1.0f / (right - left);
	float invHeight = 1.0f / (top - bottom);
	float invDepth  = 1.0f / (zFar - zNear);

	m[0]  = 2.0f * invWidth;
	m[5]  = 2.0f * invHeight;
	m[10] = 2.0f * invDepth;
	m[12] = -(right + left) * invWidth;
	m[13] = -(top + bottom) * invHeight;
	m[14] = -(zFar + zNear) * invDepth;
	return *this;
}


//-----------------------------------------------------------------------------
// Matrix factory functions
//-----------------------------------------------------------------------------

Matrix4f Matrix4f::CreateIdentity()
{
	Matrix4f result;
	result.SetIdentity();
	return result;
}

Matrix4f Matrix4f::CreateTranslation(float x, float y, float z)
{
	Matrix4f result;
	result.InitTranslation(x, y, z);
	return result;
}

Matrix4f Matrix4f::CreateTranslation(const Vector3f& translation)
{
	Matrix4f result;
	result.InitTranslation(translation);
	return result;
}

Matrix4f Matrix4f::CreateScale(float scale)
{
	Matrix4f result;
	result.InitScale(scale, scale, scale);
	return result;
}

Matrix4f Matrix4f::CreateScale(float x, float y, float z)
{
	Matrix4f result;
	result.InitScale(x, y, z);
	return result;
}

Matrix4f Matrix4f::CreateScale(const Vector3f& vScale)
{
	Matrix4f result;
	result.InitScale(vScale);
	return result;
}

Matrix4f Matrix4f::CreateRotation(const Vector3f& vAxis, float angle)
{
	Matrix4f result;
	result.InitRotation(vAxis, angle);
	return result;
}

Matrix4f Matrix4f::CreateRotation(const Quaternion& qRotation)
{
	Matrix4f result;
	result.InitRotation(qRotation);
	return result;
}

Matrix4f Matrix4f::CreatePerspective(float fovY, float aspect, float znear, float zfar)
{
	Matrix4f result;
	result.InitPerspectiveY(fovY, aspect, znear, zfar);
	return result;
}

Matrix4f Matrix4f::CreatePerspectiveX(float fovX, float aspect, float znear, float zfar)
{
	Matrix4f result;
	result.InitPerspectiveX(fovX, aspect, znear, zfar);
	return result;
}

Matrix4f Matrix4f::CreatePerspectiveXY(float fovX, float fovY, float znear, float zfar)
{
	Matrix4f result;
	result.InitPerspectiveXY(fovX, fovY, znear, zfar);
	return result;
}

Matrix4f Matrix4f::CreateOrthographic(float left, float right, float bottom, float top, float znear, float zfar)
{
	Matrix4f result;
	result.InitOrthographic(left, right, bottom, top, znear, zfar);
	return result;
}


//-----------------------------------------------------------------------------
// Static matrix operations
//-----------------------------------------------------------------------------

// Multiply 2 matrices.
void Matrix4f::Multiply(const Matrix4f& a, const Matrix4f& b, Matrix4f& dst)
{
	dst.m[0] = (b.m[0] * a.m[0]) + (b.m[1] * a.m[4]) + (b.m[2] * a.m[8])  + (b.m[3] * a.m[12]);
	dst.m[1] = (b.m[0] * a.m[1]) + (b.m[1] * a.m[5]) + (b.m[2] * a.m[9])  + (b.m[3] * a.m[13]);
	dst.m[2] = (b.m[0] * a.m[2]) + (b.m[1] * a.m[6]) + (b.m[2] * a.m[10]) + (b.m[3] * a.m[14]);
	dst.m[3] = (b.m[0] * a.m[3]) + (b.m[1] * a.m[7]) + (b.m[2] * a.m[11]) + (b.m[3] * a.m[15]);

	dst.m[4] = (b.m[4] * a.m[0]) + (b.m[5] * a.m[4]) + (b.m[6] * a.m[8])  + (b.m[7] * a.m[12]);
	dst.m[5] = (b.m[4] * a.m[1]) + (b.m[5] * a.m[5]) + (b.m[6] * a.m[9])  + (b.m[7] * a.m[13]);
	dst.m[6] = (b.m[4] * a.m[2]) + (b.m[5] * a.m[6]) + (b.m[6] * a.m[10]) + (b.m[7] * a.m[14]);
	dst.m[7] = (b.m[4] * a.m[3]) + (b.m[5] * a.m[7]) + (b.m[6] * a.m[11]) + (b.m[7] * a.m[15]);

	dst.m[8]  = (b.m[8] * a.m[0]) + (b.m[9] * a.m[4]) + (b.m[10] * a.m[8])  + (b.m[11] * a.m[12]);
	dst.m[9]  = (b.m[8] * a.m[1]) + (b.m[9] * a.m[5]) + (b.m[10] * a.m[9])  + (b.m[11] * a.m[13]);
	dst.m[10] = (b.m[8] * a.m[2]) + (b.m[9] * a.m[6]) + (b.m[10] * a.m[10]) + (b.m[11] * a.m[14]);
	dst.m[11] = (b.m[8] * a.m[3]) + (b.m[9] * a.m[7]) + (b.m[10] * a.m[11]) + (b.m[11] * a.m[15]);

	dst.m[12] = (b.m[12] * a.m[0]) + (b.m[13] * a.m[4]) + (b.m[14] * a.m[8])  + (b.m[15] * a.m[12]);
	dst.m[13] = (b.m[12] * a.m[1]) + (b.m[13] * a.m[5]) + (b.m[14] * a.m[9])  + (b.m[15] * a.m[13]);
	dst.m[14] = (b.m[12] * a.m[2]) + (b.m[13] * a.m[6]) + (b.m[14] * a.m[10]) + (b.m[15] * a.m[14]);
	dst.m[15] = (b.m[12] * a.m[3]) + (b.m[13] * a.m[7]) + (b.m[14] * a.m[11]) + (b.m[15] * a.m[15]);
}

// Return the multiplication of 2 matrices.
Matrix4f Matrix4f::Multiply(const Matrix4f& left, const Matrix4f& right)
{
	Matrix4f result;
	Multiply(left, right, result);
	return result;
}

// Return the interpolation between 2 matrices.
void Matrix4f::Lerp(const Matrix4f& a, const Matrix4f& b, float t, Matrix4f& outResult)
{
	for (int i = 0; i < 16; ++i)
		outResult.m[i] = (a.m[i] * (1.0f - t)) + (b.m[i] * t);
}

// Return the interpolation between 2 matrices.
Matrix4f Matrix4f::Lerp(const Matrix4f& a, const Matrix4f& b, float t)
{
	Matrix4f result;
	Matrix4f::Lerp(a, b, t, result);
	return result;
}

// Transpose the matrix.
void Matrix4f::Transpose(const Matrix4f& src, Matrix4f& dst)
{
	if (&src == &dst)
	{
		dst.Transpose();
	}
	else
	{
		dst.m[0] = src.m[0];  dst.m[4] = src.m[1];  dst.m[8]  = src.m[2];  dst.m[12] = src.m[3];
		dst.m[1] = src.m[4];  dst.m[5] = src.m[5];  dst.m[9]  = src.m[6];  dst.m[13] = src.m[7];
		dst.m[2] = src.m[8];  dst.m[6] = src.m[9];  dst.m[10] = src.m[10]; dst.m[14] = src.m[11];
		dst.m[3] = src.m[12]; dst.m[7] = src.m[13]; dst.m[11] = src.m[14]; dst.m[15] = src.m[15];
	}
}

void Matrix4f::Invert(const Matrix4f& inMat, Matrix4f& outResult)
{
	assert(false);
}

void Matrix4f::InvertAffine(const Matrix4f& inMat, Matrix4f& outResult)
{
	// affine transformation 4x4 matrix inverse:
	// http://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform

	// Invert the 3x3 matrix.
	Matrix3f inverse3x3;
	Matrix3f::Invert(inMat.Get3x3(), inverse3x3);
	
	// Adjust the translation vector.
	Vector3f newTranslation;
	inverse3x3.TransformVector(-inMat.GetTranslation(), newTranslation);

	// Update the resulting, inverted matrix.
	outResult.Set3x3(inverse3x3);
	outResult.SetTranslation(newTranslation);
	outResult.m[3] = 0;
	outResult.m[7] = 0;
	outResult.m[11] = 0;
	outResult.m[15] = 1;
}


//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

void Matrix4f::DoInitPerspective(float x1, float y2, float zNear, float zFar)
{
	SetIdentity();
	m[0]  = x1;
	m[5]  = y2;
	m[10] = (-zNear - zFar) / (zFar - zNear);
	m[15] = 0;
	m[14] = (-2 * zFar * zNear) / (zFar - zNear);
	m[11] = -1;
}

