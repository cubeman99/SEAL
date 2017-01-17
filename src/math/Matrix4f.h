#ifndef _MATRIX_4F_H_
#define _MATRIX_4F_H_


struct Vector3f;
struct Vector4f;
struct Quaternion;


// A 4x4 floating-point matrix.
// Column vector matrix.
// Compoents are stored internally in row-major order.
struct Matrix4f
{
	float m[4][4]; // m[row][column]

	// Constants.
	static const Matrix4f IDENTITY;

	// Constructors.
	Matrix4f();
	Matrix4f(const float* data);
	Matrix4f(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);
	Matrix4f(const Vector3f& vForward, const Vector3f& vLeft, const Vector3f& vUp);
	Matrix4f(const Vector3f& vForward, const Vector3f& vLeft, const Vector3f& vUp, const Vector3f& vTranslation);

	// Component accessors.
	float*			operator [](int row);
	const float*	operator [](int row) const;
	float&			Component(int index);
	float			Component(int index) const;
	float*			data();
	const float*	data() const;

	// Affine transformations.
	bool			IsAffine() const;

	// Affine columns.
	Vector3f		GetColumn(int columnIndex) const;
	Vector3f		GetForward() const;
	Vector3f		GetLeft() const;
	Vector3f		GetUp() const;
	Vector3f		GetTranslationColumn() const;
	void			SetColumn(int columnIndex, const Vector4f& vColumn);
	void			SetBasisVectors(const Vector3f& right, const Vector3f& up, const Vector3f& back);
	void			SetRight(const Vector3f& right);
	void			SetUp(const Vector3f& up);
	void			SetBack(const Vector3f& back);
	void			SetTranslationColumn(const Vector3f& translation);
	
	// Affine 3x3 matrix.
	Quaternion		GetRotation() const;
	
	// Mutators.
	void			SetZero();
	void			Set(float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33);

	// Matrix operations.
	Matrix4f		GetTranspose() const;
	Matrix4f&		Transpose();
	Matrix4f		operator -() const;
	Matrix4f&		Negate();
	
	// Matrix to Matrix operations.
	Matrix4f&		operator =(const Matrix4f& other);
	void			operator +=(const Matrix4f& other);
	void			operator -=(const Matrix4f& other);
	void			operator *=(const Matrix4f& other);
	bool			operator ==(const Matrix4f& other) const;
	inline bool		operator !=(const Matrix4f& other) const { return !this->operator==(other); }
	Matrix4f		operator +(const Matrix4f& other) const;
	Matrix4f		operator -(const Matrix4f& other) const;
	Matrix4f		operator *(const Matrix4f& other) const;
	Matrix4f		operator *(float scalar) const;

	// Matrix to Vector operations.
	void			Multiply3x3(const Vector3f& vIn, Vector3f& vOut) const;
	void			Multiply4x3(const Vector3f& vIn, Vector3f& vOut) const;
	void			ApplyTransform(const Vector3f& vIn, Vector3f& vOut) const;
	void			ApplyTransform(const Vector4f& vIn, Vector4f& vOut) const;
	void			ApplyRotation(const Vector3f& vIn, Vector3f& vOut) const;
	Vector3f		Multiply3x3(const Vector3f& vVec) const;
	Vector3f		Multiply4x3(const Vector3f& vVec) const;
	Vector3f		ApplyTransform(const Vector3f& vVec) const;
	Vector4f		ApplyTransform(const Vector4f& vVec) const;
	Vector3f		ApplyRotation(const Vector3f& vVec) const;
	Vector3f		operator *(const Vector3f& vVec) const;
	Vector4f		operator *(const Vector4f& vVec) const;

	// Matrix creation.
	Matrix4f&		SetIdentity();
	Matrix4f&		InitTranslation(float x, float y, float z);
	Matrix4f&		InitTranslation(const Vector3f& translation);
	Matrix4f&		InitScale(float scalar);
	Matrix4f&		InitScale(float x, float y, float z);
	Matrix4f&		InitScale(const Vector3f& scale);
	Matrix4f&		InitRotation(const Vector3f& axis, float angle);
	Matrix4f&		InitRotation(const Quaternion& qRotation);
	Matrix4f&		InitPerspectiveX(float fovX, float aspect, float zNear, float zFar);
	Matrix4f&		InitPerspectiveY(float fovY, float aspect, float zNear, float zFar);
	Matrix4f&		InitPerspectiveXY(float fovX, float fovY, float zNear, float zFar);
	Matrix4f&		InitOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);

	// Factory matrix creation.
	static Matrix4f	CreateIdentity();
	static Matrix4f	CreateTranslation(float x, float y, float z);
	static Matrix4f	CreateTranslation(const Vector3f& translation);
	static Matrix4f	CreateScale(float scale);
	static Matrix4f	CreateScale(float x, float y, float z);
	static Matrix4f	CreateScale(const Vector3f& scale);
	static Matrix4f	CreateRotation(const Vector3f& axis, float angle);
	static Matrix4f	CreateRotation(const Quaternion& rotation);
	static Matrix4f	CreatePerspective(float fovY, float aspect, float zNear, float zFar);
	static Matrix4f	CreatePerspectiveX(float fovX, float aspect, float zNear, float zFar);
	static Matrix4f	CreatePerspectiveXY(float fovX, float fovY, float zNear, float zFar);
	static Matrix4f	CreateOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	
	// Static matrix operations.
	static Matrix4f	Lerp(const Matrix4f& a, const Matrix4f& b, float lerpFactor);
	static Matrix4f	Multiply(const Matrix4f& left, const Matrix4f& right);
	static void		Multiply(const Matrix4f& left, const Matrix4f& right, Matrix4f& result);
	static void		Transpose(const Matrix4f& src, Matrix4f& dst);

private:
	void DoInitPerspective(float m00, float m11, float zNear, float zFar);
};


#endif // _MATRIX_4F_H_