#include "ArcBallCamera.h"
#include <math/MathLib.h>


ArcBallCamera::ArcBallCamera() :
	m_orientation(Quaternion::IDENTITY),
	m_parentOrientation(Quaternion::IDENTITY),
	m_upVector(Vector3f::UP)
{

}


void ArcBallCamera::SetOrientation(const Quaternion& orientation)
{
	m_orientation = orientation;
}

void ArcBallCamera::SetParentOrientation(const Quaternion& parentOrientation)
{
	m_parentOrientation = parentOrientation;
}

void ArcBallCamera::SetCenterPosition(const Vector3f& centerPosition)
{
	m_centerPosition = centerPosition;
}

void ArcBallCamera::SetUpVector(const Vector3f& upVector)
{
	m_upVector = upVector;
}

void ArcBallCamera::SetDistance(float distance)
{
	m_distance = distance;
}

void ArcBallCamera::SetMinDistance(float minDistance)
{
	m_minDistance = minDistance;
}

void ArcBallCamera::SetMaxDistance(float maxDistance)
{
	m_maxDistance = maxDistance;
}


const Vector3f& ArcBallCamera::GetCenterPosition() const
{
	return m_centerPosition;
}


Vector3f ArcBallCamera::GetViewPosition() const
{
	return (m_centerPosition - (GetOrientation().GetForward() * m_distance));
}
	
Quaternion ArcBallCamera::GetOrientation() const
{
	Quaternion orientation = m_orientation;
	orientation.Rotate(m_parentOrientation);
	return orientation;
}
	
Matrix4f ArcBallCamera::GetProjection() const
{
	return m_projection;
}
	
void ArcBallCamera::SetProjection(const Matrix4f& projection)
{
	m_projection = projection;
}

Matrix4f ArcBallCamera::GetViewProjection() const
{
	Matrix4f viewMatrix = Matrix4f::CreateRotation(GetOrientation().GetConjugate()) *
		Matrix4f::CreateTranslation(-GetViewPosition());
	return (m_projection * viewMatrix);
}




void ArcBallCamera::Rotate(float horizontal, float vertical)
{
	float rotateSpeed = Math::TWO_PI * 1.0f;

	m_orientation.Rotate(m_upVector, horizontal * rotateSpeed);
	m_orientation.Rotate(m_orientation.GetRight(), vertical * rotateSpeed);
}

void ArcBallCamera::AltRotate(float horizontal, float vertical)
{
	Rotate(horizontal, vertical);
}

void ArcBallCamera::Zoom(float amount)
{
	m_distance = Math::Clamp(
		m_distance * Math::Pow(1.1f, (float) -amount),
		m_minDistance, m_maxDistance);
}
