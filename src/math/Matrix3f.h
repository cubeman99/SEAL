#ifndef _MATRIX_3F_H_
#define _MATRIX_3F_H_

struct Vector2f;
struct Vector3f;
struct Quaternion;


// A 3x3 floating-point matrix.
// Compoents are stored in column-major order.
struct Matrix3f
{
	float m[9]; // values stored in column major order.

	// Constants.
	static const Matrix3f IDENTITY;

	// Constructors.
	Matrix3f();
	explicit Matrix3f(const float* data);
	explicit Matrix3f(	float x1, float y1, float z1,
						float x2, float y2, float z2,
						float x3, float y3, float z3);
	explicit Matrix3f(	const Vector3f& xColumn,
						const Vector3f& yColumn,
						const Vector3f& zColumn);
	
	// Accessors.
	float&			operator [](int index);
	float			operator [](int index) const;
	float*			data();
	const float*	data() const;

	// Mutators.
	void			SetZero();
	Matrix3f&		SetIdentity();
	void			Set(float x1, float y1, float z1,
						float x2, float y2, float z2,
						float x3, float y3, float z3);
	void			SetColumn(int columnIndex, const Vector3f& columnVector);
	
	// Matrix operations.
	float			GetDeterminant() const;
	Matrix3f&		Transpose();
	Matrix3f		GetTranspose() const;
	Matrix3f&		Negate();
	Matrix3f		operator -() const;
	Matrix3f&		Invert();
	Matrix3f		GetInverse() const;
	
	// Matrix to Matrix operations.
	Matrix3f&		operator =(const Matrix3f& other);
	void			operator +=(const Matrix3f& other);
	void			operator -=(const Matrix3f& other);
	void			operator *=(const Matrix3f& other);
	bool			operator ==(const Matrix3f& other) const;
	bool			operator !=(const Matrix3f& other) const;
	Matrix3f		operator +(const Matrix3f& other) const;
	Matrix3f		operator -(const Matrix3f& other) const;
	Matrix3f		operator *(const Matrix3f& other) const;
	Matrix3f		operator *(float scalar) const;

	// Matrix to Vector operations.
	void			TransformVector(const Vector2f& inVec, Vector2f& outVec) const;
	Vector2f		TransformVector(const Vector2f& vec) const;
	void			TransformVector(const Vector3f& inVec, Vector3f& outVec) const;
	Vector3f		TransformVector(const Vector3f& vec) const;
	void			RotateVector(const Vector2f& inVec, Vector2f& outVec) const;
	Vector2f		RotateVector(const Vector2f& vec) const;
	void			RotateVector(const Vector3f& inVec, Vector3f& outVec) const;
	Vector3f		RotateVector(const Vector3f& vec) const;
	void			Multiply2x2(const Vector2f& inVec, Vector2f& outVec) const;
	Vector2f		Multiply2x2(const Vector2f& vec) const;
	Vector2f		operator *(const Vector2f& vec) const;
	Vector3f		operator *(const Vector3f& vec) const;

	// Matrix creation.
	Matrix3f& InitTranslation(float x, float y);
	Matrix3f& InitTranslation(const Vector2f& translation);
	Matrix3f& InitScale(float scalar);
	Matrix3f& InitScale(float x, float y);
	Matrix3f& InitScale(float x, float y, float z);
	Matrix3f& InitScale(const Vector2f& scale);
	Matrix3f& InitScale(const Vector3f& scale);
	Matrix3f& InitRotation(float angle);
	Matrix3f& InitRotation(const Vector2f& origin, float angle);
	Matrix3f& InitRotation(const Vector3f& axis, float angle);
	Matrix3f& InitRotation(const Quaternion& rotation);
	Matrix3f& InitLookAt(const Vector3f& forward, const Vector3f& up);
	static Matrix3f	CreateIdentity();
	static Matrix3f	CreateTranslation(float x, float y);
	static Matrix3f	CreateTranslation(const Vector2f& translation);
	static Matrix3f	CreateScale(float scale);
	static Matrix3f	CreateScale(float x, float y);
	static Matrix3f	CreateScale(float x, float y, float z);
	static Matrix3f	CreateScale(const Vector2f& scale);
	static Matrix3f	CreateScale(const Vector3f& scale);
	static Matrix3f	CreateRotation(float angle);
	static Matrix3f	CreateRotation(const Vector2f& origin, float angle);
	static Matrix3f	CreateRotation(const Vector3f& axis, float angle);
	static Matrix3f	CreateRotation(const Quaternion& rotation);
	static Matrix3f	CreateLookAt(const Vector3f& forward, const Vector3f& up);
	
	// Static matrix operations.
	static Matrix3f	Multiply(const Matrix3f& left, const Matrix3f& right);
	static void		Multiply(const Matrix3f& left, const Matrix3f& right, Matrix3f& outResult);
	static Matrix3f	Lerp(const Matrix3f& a, const Matrix3f& b, float t);
	static void		Lerp(const Matrix3f& a, const Matrix3f& b, float t, Matrix3f& outResult);
	static void		Transpose(const Matrix3f& inMat, Matrix3f& outResult);
	static void		Invert(const Matrix3f& inMat, Matrix3f& outResult);
};


#endif // _MATRIX_3F_H_