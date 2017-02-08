#ifndef _ARC_BALL_CAMERA_H_
#define _ARC_BALL_CAMERA_H_

#include <graphics/ICamera.h>


//-----------------------------------------------------------------------------
// ArcBallCamera - Camera which can rotate around a point (3rd person camera).
//-----------------------------------------------------------------------------
class ArcBallCamera : public ICamera
{
public:
	ArcBallCamera();
	
	void SetParentOrientation(const Quaternion& parentorientation);
	void SetOrientation(const Quaternion& orientation);
	void SetCenterPosition(const Vector3f& centerPosition);
	void SetUpVector(const Vector3f& upVector);
	void SetDistance(float distance);
	void SetMinDistance(float minDistance);
	void SetMaxDistance(float maxDistance);

	const Vector3f& GetCenterPosition() const;

	Vector3f GetViewPosition() const override;
	Quaternion GetOrientation() const override;
	Matrix4f GetProjection() const override;
	void SetProjection(const Matrix4f& projection) override;
	Matrix4f GetViewProjection() const override;
	
	void Rotate(float horizontal, float vertical) override;
	void AltRotate(float horizontal, float vertical) override;
	void Zoom(float amount) override;

private:
	Matrix4f	m_projection;
	Quaternion	m_orientation;
	float		m_distance;
	Vector3f	m_centerPosition;
	Vector3f	m_upVector;
	float		m_minDistance;
	float		m_maxDistance;

	Quaternion	m_parentOrientation;
};


#endif // _ARC_BALL_CAMERA_H_