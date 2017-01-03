#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <math/Vector3f.h>
#include <math/Matrix4f.h>
#include <math/Quaternion.h>


class ICamera
{
public:
	virtual Quaternion GetOrientation() const = 0;
	virtual Vector3f GetViewPosition() const = 0;
	virtual Matrix4f GetViewProjection() const = 0;
	virtual Matrix4f GetProjection() const = 0;
	virtual void SetProjection(const Matrix4f& projection) = 0;

	virtual void Rotate(float horizontal, float vertical) = 0;
	virtual void AltRotate(float horizontal, float vertical) = 0;
	virtual void Zoom(float amount) = 0;
};


class Camera : public ICamera
{
public:
	Matrix4f	projection;
	Vector3f	position;
	Quaternion	rotation;

	Vector3f GetViewPosition() const  override
	{
		return position;
	}
	
	Quaternion GetOrientation() const override
	{
		return rotation;
	}
	
	void SetProjection(const Matrix4f& projection) override
	{
		this->projection = projection;
	}
	
	Matrix4f GetProjection() const override
	{
		return projection;
	}

	Matrix4f GetViewProjection() const override
	{
		Matrix4f viewMatrix = Matrix4f::CreateRotation(rotation.GetConjugate()) *
			Matrix4f::CreateTranslation(-position);
		return (projection * viewMatrix);
	}
};


#endif // _CAMERA_H_