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
	  m_simulation(nullptr),
	  m_selectedAgent(nullptr),
	  m_isSimulationPaused(false),
	  m_viewWireFrameMode(false)
{
}

SimulationManager::~SimulationManager()
{
	delete m_simulation;
}

void SimulationManager::Initialize()
{
	m_simulation = new Simulation();

	m_cameraSystem.Initialize(m_simulation);
}
		
void SimulationManager::ToggleCameraTracking()
{
	if (m_cameraSystem.IsTrackingAgent())
		m_cameraSystem.StopTrackingAgent();
	else if (m_selectedAgent != NULL)
		m_cameraSystem.StartTrackingAgent(m_selectedAgent);
}
		
void SimulationManager::StartCameraTracking()
{
	m_cameraSystem.StartTrackingAgent(m_selectedAgent);
}

void SimulationManager::StopCameraTracking()
{
	m_cameraSystem.StopTrackingAgent();
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

	// Update camera system.
	m_cameraSystem.Update();

	// TEMP: select the first agent.
	//AgentSystem* agentSystem = m_simulation->GetAgentSystem();
	//m_selectedAgent = NULL;
	//for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	//{
	//	Agent* agent = *it;
	//	m_selectedAgent = agent;
	//	break;
	//}
}
