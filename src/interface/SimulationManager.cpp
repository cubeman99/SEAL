#include "SimulationManager.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Simulation.h>


// ----------------------------------------------------------------------------
// SimulationManager
// ----------------------------------------------------------------------------

SimulationManager::SimulationManager() :
	  m_selectedAgent(NULL),
	  m_cameraTracking(false),
	  m_isSimulationPaused(false)
{
	m_simulation = new Simulation();

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	m_globeCamera.SetGlobePosition(Vector3f::ZERO);
	m_globeCamera.SetGlobeOrientation(Quaternion::IDENTITY);
	m_globeCamera.SetGlobeRadius(worldRadius);
	m_globeCamera.SetSurfaceDistance(worldRadius * 0.8f);
	m_globeCamera.SetSurfaceAngle(0.0f);

	m_arcBallCamera.SetDistance(worldRadius * 2.0f);
	m_arcBallCamera.SetUpVector(Vector3f::UP);
	m_arcBallCamera.SetCenterPosition(Vector3f::ZERO);
	m_arcBallCamera.SetOrientation(Quaternion::IDENTITY);

	//m_camera = &m_arcBallCamera;
	m_camera = &m_globeCamera;
}

SimulationManager::~SimulationManager()
{
	delete m_simulation;
}
		
void SimulationManager::ToggleCameraTracking()
{
	if (m_cameraTracking)
		StopCameraTracking();
	else if (m_selectedAgent != NULL)
		StartCameraTracking();
}
		
void SimulationManager::StartCameraTracking()
{
	m_cameraTracking = true;
	m_camera = &m_arcBallCamera;
	
	float worldRadius = m_simulation->GetWorld()->GetRadius();

	m_arcBallCamera.SetDistance(worldRadius * 0.5f);
	m_arcBallCamera.SetUpVector(Vector3f::UP);
	m_arcBallCamera.SetCenterPosition(m_selectedAgent->GetPosition());

	Quaternion orientation = Quaternion::IDENTITY;
	orientation.Rotate(Vector3f::RIGHT, Math::HALF_PI * 0.5f);
	m_arcBallCamera.SetOrientation(orientation);
}

void SimulationManager::StopCameraTracking()
{
	m_cameraTracking = false;
	m_camera = &m_globeCamera;
}

void SimulationManager::PauseSimulation()
{
	m_isSimulationPaused = !m_isSimulationPaused;
}

void SimulationManager::Update()
{
	// Update simulation.
	if (!m_isSimulationPaused)
		m_simulation->Tick();

	// Update camera tracking.
	if (m_cameraTracking && m_selectedAgent != NULL)
	{
		m_arcBallCamera.SetCenterPosition(m_selectedAgent->GetPosition());
		m_arcBallCamera.SetParentOrientation(m_selectedAgent->GetOrientation());
	}

	AgentSystem* agentSystem = m_simulation->GetAgentSystem();
	m_selectedAgent = NULL;
	for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	{
		Agent* agent = *it;
		m_selectedAgent = agent;
		break;
	}
}
