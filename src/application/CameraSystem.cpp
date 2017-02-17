#include "CameraSystem.h"
#include <simulation/Agent.h>
#include <math/MathLib.h>


CameraSystem::CameraSystem() :
	m_simulation(nullptr),
	m_cameraTracking(false),
	m_trackedObject(nullptr),
	m_aspectRatio(1.0f),
	m_fieldOfView(1.4f),
	m_isProjectionDirty(true)
{
	m_camera = &m_globeCamera;
}

void CameraSystem::Initialize(Simulation* simulation)
{
	m_simulation = simulation;
	
	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	m_globeCamera.SetGlobePosition(Vector3f::ZERO);
	m_globeCamera.SetGlobeOrientation(Quaternion::IDENTITY);
	m_globeCamera.SetGlobeRadius(worldRadius);
	m_globeCamera.SetSurfaceDistance(worldRadius * 0.8f);
	m_globeCamera.SetSurfaceAngle(0.0f);

	m_arcBallCamera.SetMinDistance(worldRadius * 0.01f);
	m_arcBallCamera.SetMaxDistance(worldRadius * 6.0f);
	m_arcBallCamera.SetDistance(worldRadius * 2.0f);
	m_arcBallCamera.SetUpVector(Vector3f::UP);
	m_arcBallCamera.SetCenterPosition(Vector3f::ZERO);
	m_arcBallCamera.SetOrientation(Quaternion::IDENTITY);
	
	m_camera = &m_globeCamera;
}

Ray CameraSystem::GetRay(const Vector2f& screenCoord)
{
	Vector3f direction = Vector3f::FORWARD;
	direction.x = screenCoord.x * m_aspectRatio;
	direction.y = screenCoord.y;
	direction.z = -(1.0f / Math::Tan(m_fieldOfView * 0.5f));
	direction.Rotate(m_camera->GetOrientation());
	direction.Normalize();

	return Ray(m_camera->GetViewPosition(), direction);
}

void CameraSystem::SetFieldOfView(float fieldOfView)
{
	m_fieldOfView = fieldOfView;
	m_isProjectionDirty = true;
}

void CameraSystem::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	m_isProjectionDirty = true;
}

void CameraSystem::StartTrackingObject(SimulationObject* object)
{
	m_cameraTracking = true;
	m_trackedObject = object;
	m_camera = &m_arcBallCamera;

	float worldRadius = m_simulation->GetWorld()->GetRadius();

	m_arcBallCamera.SetDistance(worldRadius * 0.5f);
	m_arcBallCamera.SetUpVector(Vector3f::UP);
	//m_arcBallCamera.SetCenterPosition(agent->GetPosition());

	Quaternion orientation = Quaternion(Vector3f::LEFT, Math::HALF_PI * 0.5f);
	m_arcBallCamera.SetOrientation(orientation);
}

void CameraSystem::StopTrackingObject()
{
	m_cameraTracking = false;
	m_trackedObject = nullptr;
	m_camera = &m_globeCamera;
}

void CameraSystem::Update()
{
	// Update camera tracking.
	if (m_cameraTracking && m_trackedObject != nullptr)
	{
		m_arcBallCamera.SetCenterPosition(m_trackedObject->GetPosition());
		m_arcBallCamera.SetParentOrientation(m_trackedObject->GetOrientation());
	}
	
	// Update camera projection.
	if (m_isProjectionDirty)
	{
		Matrix4f projection = Matrix4f::CreatePerspective(
			m_fieldOfView, m_aspectRatio, 0.1f, m_simulation->GetWorld()->GetRadius() * 5.0f);
		
		m_arcBallCamera.SetProjection(projection);
		m_globeCamera.SetProjection(projection);

		m_isProjectionDirty = false;
	}
}
