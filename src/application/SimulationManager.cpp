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
	  m_showAgentVision(false),
	  m_showAgentBrain(false),
	  m_showInvisibleObjects(false)
{
}

SimulationManager::~SimulationManager()
{
	delete m_simulation;
}

void SimulationManager::Initialize()
{
	SimulationConfig config; // TEMP: use default config values.
	m_simulation = new Simulation();
	m_simulation->Initialize(config);

	// Initialize systems.
	m_cameraSystem.Initialize(m_simulation);
	m_simulationRenderer.Initialize(this);
	
	// Seed rand with current time
	Random::SeedTime();
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
}
