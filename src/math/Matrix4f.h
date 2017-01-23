#ifndef _MATRIX_4F_H_
#define _MATRIX_4F_H_

struct Vector3f;
struct Vector4f;
struct Quaternion;
struct Matrix3f;


// A 4x4 floating-point matrix.
// Multiplication happens as if it is a column vector matrix.
// Componets are stored internally in column-major order.
struct Matrix4f
{
	float m[16]; // values stored in column major order.

	// Constants
	static const Matrix4f IDENTITY;

	// Constructors
	Matrix4f();
	explicit Matrix4f(const float* data);
	explicit Matrix4f(	float x1, float y1, float z1, float w1,
						float x2, float y2, float z2, float w2,
						float x3, float y3, float z3, float w3,
						float x4, float y4, float z4, float w4);
	explicit Matrix4f(	const Vector3f& xBasis,
						const Vector3f& yBasis,
						const Vector3f& zBasis);
	explicit Matrix4f(	const Vector3f& xBasis,
						const Vector3f& yBasis,
						const Vector3f& zBasis,
						const Vector3f& translation);
	explicit Matrix4f(	const Vector4f& xColumn,
						const Vector4f& yColumn,
						const Vector4f& zColumn,
						const Vector4f& wColumn);
	explicit Matrix4f(const Matrix3f& mat3x3);

	// Accessors
	float&			operator [](int index);
	float			operator [](int index) const;
	float*			data();
	const float*	data() const;
	float			GetDeterminant() const;
	Vector4f		GetColumn(int columnIndex) const;
	
	// Mutators
	Matrix4f&		SetZero();
	Matrix4f&		SetIdentity();
	void			Set(float x1, float y1, float z1, float w1,
						float x2, float y2, float z2, float w2,
						float x3, float y3, float z3, float w3,
						float x4, float y4, float z4, float w4);
	void			SetColumn(int columnIndex, const Vector4f& columnVector);

	// Affine transformations
	bool			IsAffine() const;
	Vector3f		GetXBasis() const;
	Vector3f		GetYBasis() const;
	Vector3f		GetZBasis() const;
	Vector3f		GetTranslation() const;
	Quaternion		GetRotation() const;
	Matrix3f		Get3x3() const;

	void			SetXBasis(const Vector3f& xBasis);
	void			SetYBasis(const Vector3f& yBasis);
	void			SetZBasis(const Vector3f& zBasis);
	void			SetTranslation(const Vector3f& translation);
	void			SetBasisVectors(const Vector3f& xBasis,
									const Vector3f& yBasis,
									const Vector3f& zBasis);
	void			Set3x3(const Matrix3f& m3x3);

	// Matrix operations
	Matrix4f&		Transpose();
	Matrix4f		GetTranspose() const;
	Matrix4f&		Negate();
	Matrix4f		operator -() const;
	Matrix4f&		Invert();
	Matrix4f		GetInverse() const;
	Matrix4f&		InvertAffine();
	Matrix4f		GetAffineInverse() const;
	
	// Matrix to Matrix operations
	Matrix4f&		operator =(const Matrix4f& other);
	void			operator +=(const Matrix4f& other);
	void			operator -=(const Matrix4f& other);
	void			operator *=(const Matrix4f& other);
	bool			operator ==(const Matrix4f& other) const;
	bool			operator !=(const Matrix4f& other) const;
	Matrix4f		operator +(const Matrix4f& other) const;
	Matrix4f		operator -(const Matrix4f& other) const;
	Matrix4f		operator *(const Matrix4f& other) const;
	Matrix4f		operator *(float scalar) const;

	// Matrix to Vector operations
	void			Multiply3x3(const Vector3f& inVec, Vector3f& outVec) const;
	void			Multiply4x3(const Vector3f& inVec, Vector3f& outVec) const;
	void			ApplyTransform(const Vector3f& inVec, Vector3f& outVec) const;
	void			ApplyTransform(const Vector4f& inVec, Vector4f& outVec) const;
	void			ApplyRotation(const Vector3f& inVec, Vector3f& outVec) const;
	Vector3f		Multiply3x3(const Vector3f& vec) const;
	Vector3f		Multiply4x3(const Vector3f& vec) const;
	Vector3f		ApplyTransform(const Vector3f& vec) const;
	Vector4f		ApplyTransform(const Vector4f& vec) const;
	Vector3f		ApplyRotation(const Vector3f& vec) const;
	Vector3f		operator *(const Vector3f& vec) const;
	Vector4f		operator *(const Vector4f& vec) const;

	// Matrix initialization
	Matrix4f& InitTranslation(float x, float y, float z);
	Matrix4f& InitTranslation(const Vector3f& translation);
	Matrix4f& InitScale(float scalar);
	Matrix4f& InitScale(float x, float y, float z);
	Matrix4f& InitScale(const Vector3f& scale);
	Matrix4f& InitRotation(const Vector3f& axis, float angle);
	Matrix4f& InitRotation(const Quaternion& rotation);
	Matrix4f& InitLookAt(const Vector3f& lookAtDir, const Vector3f& upDir);
	Matrix4f& InitPerspectiveX(	float fovX, float aspect,
								float zNear, float zFar);
	Matrix4f& InitPerspectiveY(	float fovY, float aspect,
								float zNear, float zFar);
	Matrix4f& InitPerspectiveXY(float fovX, float fovY,
								float zNear, float zFar);
	Matrix4f& InitOrthographic(float left, float right,
									float bottom, float top,
									float zNear, float zFar);

	// Matrix factory functions
	static Matrix4f	CreateIdentity();
	static Matrix4f	CreateTranslation(float x, float y, float z);
	static Matrix4f	CreateTranslation(const Vector3f& translation);
	static Matrix4f	CreateScale(float scale);
	static Matrix4f	CreateScale(float x, float y, float z);
	static Matrix4f	CreateScale(const Vector3f& scale);
	static Matrix4f	CreateRotation(const Vector3f& axis, float angle);
	static Matrix4f	CreateRotation(const Quaternion& rotation);
	static Matrix4f	CreatePerspective(	float fovY, float aspect,
										float zNear, float zFar);
	static Matrix4f	CreatePerspectiveX(	float fovX, float aspect,
										float zNear, float zFar);
	static Matrix4f	CreatePerspectiveXY(float fovX, float fovY,
										float zNear, float zFar);
	static Matrix4f	CreateOrthographic(	float left, float right,
										float bottom, float top,
										float znear, float zfar);
	
	// Static matrix operations
	static void		Multiply(const Matrix4f& left, const Matrix4f& right, Matrix4f& outMat);
	static Matrix4f	Multiply(const Matrix4f& left, const Matrix4f& right);
	static void		Lerp(const Matrix4f& a, const Matrix4f& b, float t, Matrix4f& outMat);
	static Matrix4f	Lerp(const Matrix4f& a, const Matrix4f& b, float t);
	static void		Transpose(const Matrix4f& inMat, Matrix4f& outMat);
	static void		Invert(const Matrix4f& inMat, Matrix4f& outMat);
	static void		InvertAffine(const Matrix4f& inMat, Matrix4f& outMat);

private:
	// Private methods
	void DoInitPerspective(float x1, float y2, float zNear, float zFar);
};


#endif // _MATRIX_4F_H_