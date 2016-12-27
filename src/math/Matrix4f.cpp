#include "Matrix4f.h"
#include "Quaternion.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "MathLib.h"


//-----------------------------------------------------------------------------
// Matrix constants.
//-----------------------------------------------------------------------------

// The 4x4 identity matrix.
const Matrix4f Matrix4f::IDENTITY (1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1);



//-----------------------------------------------------------------------------
// Matrix constructors.
//-----------------------------------------------------------------------------

// Construct the matrix with uninitialized components.
Matrix4f::Matrix4f()
{
}

// Construct the matrix from data stored in row-major order.
Matrix4f::Matrix4f(const float* data)
{
	memcpy(&m[0][0], data, sizeof(Matrix4f));
}

// Construct the matrix with the given components.
Matrix4f::Matrix4f(float  m00, float m01, float m02, float m03,
					float  m10, float m11, float m12, float m13,
					float  m20, float m21, float m22, float m23,
					float  m30, float m31, float m32, float m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

// Creates a matrix where the X-axis = forward,
// the Y-axis = left, and the Z-axis = up.
Matrix4f::Matrix4f(const Vector3f& vForward, const Vector3f& vLeft, const Vector3f& vUp)
{
	Set(vForward.x, vLeft.x, vUp.x, 0.0f,
		vForward.y, vLeft.y, vUp.y, 0.0f,
		vForward.z, vLeft.z, vUp.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

// Creates a matrix with a translation where the
// X-axis = forward, the Y-axis = left, and the Z-axis = up.
Matrix4f::Matrix4f(const Vector3f& vForward, const Vector3f& vLeft, const Vector3f& vUp, const Vector3f& vTranslation)
{
	Set(vForward.x, vLeft.x, vUp.x, vTranslation.x,
		vForward.y, vLeft.y, vUp.y, vTranslation.y,
		vForward.z, vLeft.z, vUp.z, vTranslation.z,
		0.0f, 0.0f, 0.0f, 1.0f);
}

// Create a 4x4 matrix from a 3x3 matrix.
//Matrix4f::Matrix4f(const Matrix3f& m3x3)
//{
//	m[0][0] = m3x3.m[0][0];	m[0][1] = m3x3.m[0][1];	m[0][2] = m3x3.m[0][2]; m[0][3] = 0.0f;
//	m[1][0] = m3x3.m[1][0];	m[1][1] = m3x3.m[1][1];	m[1][2] = m3x3.m[1][2]; m[1][3] = 0.0f;
//	m[2][0] = m3x3.m[2][0];	m[2][1] = m3x3.m[2][1];	m[2][2] = m3x3.m[2][2]; m[2][3] = 0.0f;
//	m[3][0] = 0.0f;			m[3][1] = 0.0f;			m[3][2] = 0.0f;			m[3][3] = 1.0f;
//}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return a pointer to a row of components.
float* Matrix4f::operator[](int row)
{
	
	assert(row >= 0 && row < 4);
	return m[row];
}

// Return a pointer to a row of components.
const float* Matrix4f::operator[](int row) const
{
	assert(row >= 0 && row < 4);
	return m[row];
}

// Return a pointer to the first component in memory.
float* Matrix4f::data()
{
	return &m[0][0];
}

// Return a pointer to the first component in memory.
const float* Matrix4f::data() const
{
	return &m[0][0];
}

// Obtain a reference to an indexed component (in row-major order).
float& Matrix4f::Component(int index)
{
	return ((float*) m)[index];
}

// Access an indexed component (in row-major order).
float Matrix4f::Component(int index) const
{
	return ((float*) m)[index];
}

bool Matrix4f::IsAffine() const
{
	// Affine transformations only use the top 3 rows.
	return (m[3][0] == 0.0f && m[3][1] == 0.0f &&
			m[3][2] == 0.0f && m[3][3] == 1.0f);
}

Vector3f Matrix4f::GetColumn(int columnIndex) const
{
	return Vector3f(
		m[0][columnIndex],
		m[1][columnIndex],
		m[2][columnIndex]);
}

// Return the forward basis vector (+X direction).
Vector3f Matrix4f::GetForward() const
{
	return GetColumn(0);
}

// Return the left basis vector (+Y direction).
Vector3f Matrix4f::GetLeft() const
{
	return GetColumn(1);
}

// Return the up basis vector (+Z direction).
Vector3f Matrix4f::GetUp() const
{
	return GetColumn(3);
}

Vector3f Matrix4f::GetTranslationColumn() const
{
	return GetColumn(3);
}

// Get the pure rotation as a quaternion from the 3x3 submatrix (if it is orthogonal).
Quaternion Matrix4f::GetRotation() const
{
	Quaternion out;
	float trace = m[0][0] + m[1][1] + m[2][2];
		
	if (trace > 0.0f)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		out.w = 0.25f / s;
		out.x = (m[1][2] - m[2][1]) * s;
		out.y = (m[2][0] - m[0][2]) * s;
		out.z = (m[0][1] - m[1][0]) * s;
	}
	else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
	{
		float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
		out.w = (m[1][2] - m[2][1]) / s;
		out.x = 0.25f * s;
		out.y = (m[1][0] + m[0][1]) / s;
		out.z = (m[2][0] + m[0][2]) / s;
	}
	else if (m[1][1] > m[2][2])
	{
		float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
		out.w = (m[2][0] - m[0][2]) / s;
		out.x = (m[1][0] + m[0][1]) / s;
		out.y = 0.25f * s;
		out.z = (m[2][1] + m[1][2]) / s;
	}
	else
	{
		float s = 2.0f * sqrtf(1.0f + m[2][2] - m[1][1] - m[0][0]);
		out.w = (m[0][1] - m[1][0]) / s;
		out.x = (m[2][0] + m[0][2]) / s;
		out.y = (m[1][2] + m[2][1]) / s;
		out.z = 0.25f * s;
	}

	out.Normalize();
	return out;
}

// Return the upper-left 3x3 submatrix.
//Matrix3f Matrix4f::Get3x3() const
//{
//	return Matrix3f(m[0][0], m[0][1], m[0][2],
//					m[1][0], m[1][1], m[1][2],
//					m[2][0], m[2][1], m[2][2]);
//}

//void Matrix4f::Set3x3(const Matrix3f& m3x3)
//{
//	m[0][0] = m3x3[0][0]; m[0][1] = m3x3[0][1]; m[0][2] = m3x3[0][2];
//	m[1][0] = m3x3[1][0]; m[1][1] = m3x3[1][1]; m[1][2] = m3x3[1][2];
//	m[2][0] = m3x3[2][0]; m[2][1] = m3x3[2][1]; m[2][2] = m3x3[2][2];
//}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set all the components of this matrix to zero.
void Matrix4f::SetZero()
{
	memset(&m[0][0], 0, sizeof(m));
}

// Set the individual components.
void Matrix4f::Set(float  m00, float m01, float m02, float m03,
					float  m10, float m11, float m12, float m13,
					float  m20, float m21, float m22, float m23,
					float  m30, float m31, float m32, float m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

// Set the components of a single column from a vector.
void Matrix4f::SetColumn(int columnIndex, const Vector4f& vColumn)
{
	m[0][columnIndex] = vColumn.x;
	m[1][columnIndex] = vColumn.y;
	m[2][columnIndex] = vColumn.z;
	m[3][columnIndex] = vColumn.w;
}

// Set the left basis column vector (+Y).
void Matrix4f::SetRight(const Vector3f& right)
{
	m[0][0] = right.x;
	m[1][0] = right.y;
	m[2][0] = right.z;
}

// Set the up basis column vector (+Z).
void Matrix4f::SetUp(const Vector3f& up)
{
	m[0][1] = up.x;
	m[1][1] = up.y;
	m[2][1] = up.z;
}

// Set the forward basis column vector (-Z).
void Matrix4f::SetBack(const Vector3f& back)
{
	m[0][2] = back.x;
	m[1][2] = back.y;
	m[2][2] = back.z;
}

// Set the three basis column vectors (+X, +Y, and +Z).
void Matrix4f::SetBasisVectors(const Vector3f& right, const Vector3f& up, const Vector3f& back)
{
	SetRight(right);	// +X
	SetUp(up);			// +Y
	SetBack(back);		// +Z
}
/*
// Set the three basis column vectors (+X, +Y, and +Z).
void Matrix4f::SetBasisVectors(const Vector3f &vForward, const Vector3f &vLeft, const Vector3f &vUp)
{
	SetForward(vForward);
	SetLeft(vLeft);
	SetUp(vUp);
}

// Set the forward basis column vector (-Z).
void Matrix4f::SetForward(const Vector3f& vForward)
{
	m[0][0] = vForward.x;
	m[1][0] = vForward.y;
	m[2][0] = vForward.z;
}

// Set the left basis column vector (+Y).
void Matrix4f::SetLeft(const Vector3f& vLeft)
{
	m[0][1] = vLeft.x;
	m[1][1] = vLeft.y;
	m[2][1] = vLeft.z;
}

// Set the up basis column vector (+Z).
void Matrix4f::SetUp(const Vector3f& vUp)
{
	m[0][2] = vUp.x;
	m[1][2] = vUp.y;
	m[2][2] = vUp.z;
}

// Set the translation column of the matrix transformation.
void Matrix4f::SetTranslationColumn(const Vector3f& vTranslation)
{
	m[0][3] = vTranslation.x;
	m[1][3] = vTranslation.y;
	m[2][3] = vTranslation.z;
}
*/


//-----------------------------------------------------------------------------
// Unary Matrix operations.
//-----------------------------------------------------------------------------

// Transpose the matrix.
Matrix4f& Matrix4f::Transpose()
{
	Matrix4f::Transpose(*this, *this);
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
		Component(i) = -Component(i);
	return *this;
}

// Return the negation of this matrix.
Matrix4f Matrix4f::operator-() const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.Component(i) = -Component(i);
	return result;
}

// Scale all components.
Matrix4f Matrix4f::operator*(float scalar) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.Component(i) = Component(i) * scalar;
	return result;
}

inline void Swap(float& a, float& b)
{
	float tmp = a;
	a = b;
	b = tmp;
}

// Transpose the matrix.
void Matrix4f::Transpose(const Matrix4f& src, Matrix4f& dst)
{
	if (&src == &dst)
	{
		Swap(dst[0][1], dst[1][0]);
		Swap(dst[0][2], dst[2][0]);
		Swap(dst[0][3], dst[3][0]);
		Swap(dst[1][2], dst[2][1]);
		Swap(dst[1][3], dst[3][1]);
		Swap(dst[2][3], dst[3][2]);
	}
	else
	{
		dst[0][0] = src[0][0]; dst[0][1] = src[1][0]; dst[0][2] = src[2][0]; dst[0][3] = src[3][0];
		dst[1][0] = src[0][1]; dst[1][1] = src[1][1]; dst[1][2] = src[2][1]; dst[1][3] = src[3][1];
		dst[2][0] = src[0][2]; dst[2][1] = src[1][2]; dst[2][2] = src[2][2]; dst[2][3] = src[3][2];
		dst[3][0] = src[0][3]; dst[3][1] = src[1][3]; dst[3][2] = src[2][3]; dst[3][3] = src[3][3];
	}
}



//-----------------------------------------------------------------------------
// Matrix to Matrix operations.
//-----------------------------------------------------------------------------

// Assign the components of this matrix from another.
Matrix4f& Matrix4f::operator=(const Matrix4f& other)
{
	m[0][0] = other.m[0][0];
	m[0][1] = other.m[0][1];
	m[0][2] = other.m[0][2];
	m[0][3] = other.m[0][3];

	m[1][0] = other.m[1][0];
	m[1][1] = other.m[1][1];
	m[1][2] = other.m[1][2];
	m[1][3] = other.m[1][3];

	m[2][0] = other.m[2][0];
	m[2][1] = other.m[2][1];
	m[2][2] = other.m[2][2];
	m[2][3] = other.m[2][3];

	m[3][0] = other.m[3][0];
	m[3][1] = other.m[3][1];
	m[3][2] = other.m[3][2];
	m[3][3] = other.m[3][3];
	return *this;
}

// Add another matrix.
void Matrix4f::operator+=(const Matrix4f& other)
{
	for (int i = 0; i < 16; ++i)
		Component(i) += other.Component(i);
}

// Subtract another matrix.
void Matrix4f::operator-=(const Matrix4f& other)
{
	for (int i = 0; i < 16; ++i)
		Component(i) -= other.Component(i);
}

// Multiply by another matrix.
void Matrix4f::operator*=(const Matrix4f& other)
{
	Matrix4f temp;
	Matrix4f::Multiply(*this, other, temp);
	*this = temp;
}

// Test if the components of this matrix equal the components of another.
bool Matrix4f::operator==(const Matrix4f& other) const
{
	return !memcmp(other.m, m, sizeof(m));
}

// Add another matrix.
Matrix4f Matrix4f::operator+(const Matrix4f& other) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.Component(i) = Component(i) + other.Component(i);
	return result;
}

// Subtract another matrix.
Matrix4f Matrix4f::operator-(const Matrix4f& other) const
{
	Matrix4f result;
	for (int i = 0; i < 16; ++i)
		result.Component(i) = Component(i) - other.Component(i);
	return result;
}

// Multiply by another matrix.
Matrix4f Matrix4f::operator*(const Matrix4f& other) const
{
	Matrix4f result;
	Matrix4f::Multiply(*this, other, result);
	return result;
}

// Return the multiplication of 2 matrices.
Matrix4f Matrix4f::Multiply(const Matrix4f& left, const Matrix4f& right)
{
	Matrix4f result;
	Multiply(left, right, result);
	return result;
}

// Multiply 2 matrices.
void Matrix4f::Multiply(const Matrix4f& a, const Matrix4f& b, Matrix4f& dst)
{
	dst[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
	dst[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
	dst[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
	dst[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

	dst[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
	dst[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
	dst[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
	dst[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

	dst[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
	dst[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
	dst[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
	dst[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

	dst[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
	dst[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
	dst[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
	dst[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
}

// Return the interpolation between 2 matrices.
Matrix4f Matrix4f::Lerp(const Matrix4f& a, const Matrix4f& b, float lerpFactor)
{
	return (a * (1.0f - lerpFactor)) + (b * lerpFactor);
}



//-----------------------------------------------------------------------------
// Matrix to Vector operations.
//-----------------------------------------------------------------------------

// Mutliply the upper-left 3x3 sub-matrix by a 3D vector (this will effectively apply a rotation).
void Matrix4f::Multiply3x3(const Vector3f& vVec, Vector3f& vOut) const
{
	vOut.x = (m[0][0] * vVec.x) + (m[0][1] * vVec.y) + (m[0][2] * vVec.z);
	vOut.y = (m[1][0] * vVec.x) + (m[1][1] * vVec.y) + (m[1][2] * vVec.z);
	vOut.z = (m[2][0] * vVec.x) + (m[2][1] * vVec.y) + (m[2][2] * vVec.z);
}

// Mutliply the upper 3 rows by a 3D vector.
void Matrix4f::Multiply4x3(const Vector3f& vVec, Vector3f& vOut) const
{
	vOut.x = (m[0][0] * vVec.x) + (m[0][1] * vVec.y) + (m[0][2] * vVec.z) + m[0][3];
	vOut.y = (m[1][0] * vVec.x) + (m[1][1] * vVec.y) + (m[1][2] * vVec.z) + m[1][3];
	vOut.z = (m[2][0] * vVec.x) + (m[2][1] * vVec.y) + (m[2][2] * vVec.z) + m[2][3];
}

// Mutliply the upper-left 3x3 sub-matrix by a 3D vector (this will effectively apply a rotation).
Vector3f Matrix4f::Multiply3x3(const Vector3f& vVec) const
{
	Vector3f result;
	Multiply3x3(vVec, result);
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
void Matrix4f::ApplyTransform(const Vector3f& vIn, Vector3f& vOut) const
{
	float rw = 1.0f / ((m[3][0] * vIn.x) + (m[3][1] * vIn.y) + (m[3][2] * vIn.z) + m[3][3]);
	vOut.x = ((m[0][0] * vIn.x) + (m[0][1] * vIn.y) + (m[0][2] * vIn.z) + m[0][3]) * rw;
	vOut.y = ((m[1][0] * vIn.x) + (m[1][1] * vIn.y) + (m[1][2] * vIn.z) + m[1][3]) * rw;
	vOut.z = ((m[2][0] * vIn.x) + (m[2][1] * vIn.y) + (m[2][2] * vIn.z) + m[2][3]) * rw;
}

// Applies a rotation to a 3D vector (ignores the translation components, this just calls Multiply3x3)
void Matrix4f::ApplyRotation(const Vector3f& vIn, Vector3f& vOut) const
{
	Multiply3x3(vIn, vOut);
	vOut.Normalize();
}

// Multiply by a 4D vector.
void Matrix4f::ApplyTransform(const Vector4f& vIn, Vector4f& vOut) const
{
	vOut[0] = (m[0][0] * vIn[0]) + (m[0][1] * vIn[1]) + (m[0][2] * vIn[2]) + (m[0][3] * vIn[3]);
	vOut[1] = (m[1][0] * vIn[0]) + (m[1][1] * vIn[1]) + (m[1][2] * vIn[2]) + (m[1][3] * vIn[3]);
	vOut[2] = (m[2][0] * vIn[0]) + (m[2][1] * vIn[1]) + (m[2][2] * vIn[2]) + (m[2][3] * vIn[3]);
	vOut[3] = (m[3][0] * vIn[0]) + (m[3][1] * vIn[1]) + (m[3][2] * vIn[2]) + (m[3][3] * vIn[3]);
}

// Multiply by a 3D vector (divides by w and assumes input w is 1)
Vector3f Matrix4f::ApplyTransform(const Vector3f& vec) const
{
	Vector3f result;
	ApplyTransform(vec, result);
	return result;
}

// Applies a rotation to a 3D vector (ignores the translation components, this just calls Multiply3x3)
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
// Matrix Creation
//-----------------------------------------------------------------------------

// Set this matrix to the identity matrix.
Matrix4f& Matrix4f::SetIdentity()
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	return *this;
}

Matrix4f& Matrix4f::InitTranslation(float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
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
	m[0][0] = x;    m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = y;    m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = z;    m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
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

Matrix4f& Matrix4f::InitRotation(const Quaternion& qRotation)
{
	SetIdentity();
	SetRight(qRotation.GetRight());
	SetUp(qRotation.GetUp());
	SetBack(qRotation.GetBack());
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveX(float fovX, float aspect, float zNear, float zFar)
{
	float invTanHalfFoV = 1.0f / Math::Tan(fovX * 0.5f);
	DoInitPerspective(
		invTanHalfFoV,
		invTanHalfFoV * aspect,
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveY(float fovY, float aspect, float zNear, float zFar)
{
	float invTanHalfFoV = 1.0f / Math::Tan(fovY * 0.5f);
	DoInitPerspective(
		invTanHalfFoV / aspect,
		invTanHalfFoV,
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitPerspectiveXY(float fovX, float fovY, float zNear, float zFar)
{
	DoInitPerspective(
		1.0f / Math::Tan(fovX * 0.5f),
		1.0f / Math::Tan(fovY * 0.5f),
		zNear, zFar);
	return *this;
}

Matrix4f& Matrix4f::InitOrthographic(float left, float right, float bottom, float top, float znear, float zfar)
{
	SetIdentity();

	float width  = right - left;
	float height = top - bottom;
	float depth  = zfar - znear;

	m[0][0] = 2.0f / width;
	m[1][1] = 2.0f / height;
	m[2][2] = 2.0f / depth;
	m[0][3] = -(right + left) / width;
	m[1][3] = -(top + bottom) / height;
	m[2][3] = -(zfar + znear) / depth;
	return *this;
}


//-----------------------------------------------------------------------------
// Matrix Factory Creation
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

Matrix4f Matrix4f::CreateTranslation(const Vector3f& vTranslation)
{
	Matrix4f result;
	result.InitTranslation(vTranslation);
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
// Private methods
//-----------------------------------------------------------------------------

void Matrix4f::DoInitPerspective(float m00, float m11, float zNear, float zFar)
{
	SetIdentity();
	m[0][0] = m00;
	m[1][1] = m11;
	m[2][2] = (-zNear - zFar) / (zFar - zNear);
	m[3][3] = 0.0f;
	m[2][3] = (-2.0f * zFar * zNear) / (zFar - zNear);
	m[3][2] = -1.0f;
}

