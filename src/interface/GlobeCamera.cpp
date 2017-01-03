#include "GlobeCamera.h"
#include <math/MathLib.h>


GlobeCamera::GlobeCamera()
{

}


void GlobeCamera::Initialize(const Vector3f& globePosition, float globeRadius,
		float surfaceDistance, float surfaceAngle)
{
	m_globePosition = globePosition;
	m_globeRadius = globeRadius;
	m_surfaceDistance = surfaceDistance;
	m_surfaceAngle = surfaceAngle;
	m_globeOrientation.SetIdentity();
}

void GlobeCamera::SetGlobeRadius(float globeRadius)
{
	m_globeRadius = globeRadius;
}

void GlobeCamera::SetGlobePosition(const Vector3f& globePosition)
{
	m_globePosition = globePosition;
}

void GlobeCamera::SetGlobeOrientation(const Quaternion& orientation)
{
	m_globeOrientation = orientation;
}

void GlobeCamera::SetSurfaceDistance(float surfaceDistance)
{
	m_surfaceDistance = surfaceDistance;
}

void GlobeCamera::SetSurfaceAngle(float surfaceAngle)
{
	m_surfaceAngle = surfaceAngle;
}


void GlobeCamera::RotateSurfaceCamera(float horizontal, float vertical)
{
	float rotateSpeed = 0.008f;

	m_globeOrientation.Rotate(m_globeOrientation.GetForward(), -horizontal * rotateSpeed);
	m_surfaceAngle -= vertical * rotateSpeed * 0.5f;
	m_surfaceAngle = Math::Clamp(m_surfaceAngle, 0.0f, Math::HALF_PI);
}

void GlobeCamera::RotateAroundGlobe(float horizontal, float vertical)
{
	float rotateSpeed = 0.01f;
	float mult = Math::Min(m_surfaceDistance / m_globeRadius, 1.0f);
	rotateSpeed *= mult;

	m_globeOrientation.Rotate(m_globeOrientation.GetUp(), horizontal * rotateSpeed);
	m_globeOrientation.Rotate(m_globeOrientation.GetRight(), vertical * rotateSpeed);
}

void GlobeCamera::Zoom(float amount)
{
	m_surfaceDistance = Math::Clamp(
		m_surfaceDistance * Math::Pow(1.1f, (float) -amount),
		m_globeRadius * 0.01f, m_globeRadius * 4.0f);
}

Vector3f GlobeCamera::GetSurfacePosition() const
{
	return (m_globePosition - (m_globeOrientation.GetForward() * m_globeRadius));
}

float GlobeCamera::GetAdjustedSurfaceAngle() const
{
	float maxDistance = m_globeRadius * 1.6f;

	if (m_surfaceDistance <= 0.0f)
		return m_surfaceAngle;
	if (m_surfaceDistance >= maxDistance)
		return 0.0f;
	
	return (m_surfaceAngle * 0.5f) * (1.0f +
		Math::Cos((m_surfaceDistance * Math::PI) / maxDistance));
}


Vector3f GlobeCamera::GetViewPosition() const
{
	Vector3f pos = m_globeOrientation.GetBack() * m_surfaceDistance;
	pos.Rotate(m_globeOrientation.GetRight(), -GetAdjustedSurfaceAngle());
	pos += m_globeOrientation.GetBack() * m_globeRadius;
	pos += m_globePosition;
	return pos;
}
	
Quaternion GlobeCamera::GetOrientation() const
{
	Quaternion orientation = m_globeOrientation;
	orientation.Rotate(orientation.GetRight(), -GetAdjustedSurfaceAngle());
	return orientation;
}
	
Matrix4f GlobeCamera::GetProjection() const
{
	return m_projection;
}
	
void GlobeCamera::SetProjection(const Matrix4f& projection)
{
	m_projection = projection;
}

Matrix4f GlobeCamera::GetViewProjection() const
{
	Vector3f pos = GetViewPosition();
	Matrix4f viewMatrix = Matrix4f::CreateRotation(GetOrientation().GetConjugate()) *
		Matrix4f::CreateTranslation(-pos);
	return (m_projection * viewMatrix);
}


