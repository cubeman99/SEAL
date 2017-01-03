#ifndef _GLOBEL_CAMERA_H_
#define _GLOBEL_CAMERA_H_

#include "ICamera.h"


class GlobeCamera : public ICamera
{
public:
	GlobeCamera();
	
	void Initialize(const Vector3f& globePosition, float globeRadius,
		float surfaceDistance, float surfaceAngle);
	
	void SetGlobeRadius(float globeRadius);
	void SetGlobePosition(const Vector3f& globePosition);
	void SetGlobeOrientation(const Quaternion& orientation);
	void SetSurfaceDistance(float surfaceDistance);
	void SetSurfaceAngle(float surfaceAngle);

	void RotateSurfaceCamera(float horizontal, float vertical);
	void RotateAroundGlobe(float horizontal, float vertical);
	void Zoom(float amount);
	Vector3f GetSurfacePosition() const;
	float GetAdjustedSurfaceAngle() const;

	Vector3f GetViewPosition() const override;
	Quaternion GetOrientation() const override;
	Matrix4f GetProjection() const override;
	void SetProjection(const Matrix4f& projection) override;
	Matrix4f GetViewProjection() const override;

private:
	Matrix4f	m_projection;
	Quaternion	m_globeOrientation;
	Vector3f	m_globePosition;
	float		m_globeRadius;
	float		m_surfaceDistance;
	float		m_surfaceAngle; // 0 = looking straight down, pi/2 = looking parallel to surface
};


#endif // _GLOBEL_CAMERA_H_