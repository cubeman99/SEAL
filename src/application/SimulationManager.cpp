#include "SimulationManager.h"
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
	  m_viewWireFrameMode(false),
	  m_viewLighting(true),
	  m_showOctTree(false),
	  m_showOctTreeWireFrame(false),
	  m_showAgentVision(false)
{
}

SimulationManager::~SimulationManager()
{
	delete m_simulation;
}

void SimulationManager::Initialize()
{
	m_simulation = new Simulation();
	m_simulation->Initialize();

	// Initialize systems.
	m_cameraSystem.Initialize(m_simulation);
	m_simulationRenderer.Initialize(this);
	
	// Seed rand with current time
	Random::SeedTime();

	// DEBUG: spawn some plants.
	for (int i = 0; i < 5; ++i)
	{
		m_simulation->GetAgentSystem()->SpawnPlant();
	}
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
