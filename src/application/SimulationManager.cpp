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
	  m_selectedAgentId(-1),
	  m_selectedAgent(nullptr),
	  m_isSimulationPaused(false),
	  m_viewWireFrameMode(false),
	  m_viewLighting(true),
	  m_showOctTree(false),
	  m_showOctTreeWireFrame(false),
	  m_showAgentVision(false),
	  m_showAgentBrain(false),
	  m_showInvisibleObjects(false),
	  m_showAxisLines(false)
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

bool SimulationManager::SaveSimulation(const std::string& fileName)
{
	bool result = m_simulation->SaveTimeline(fileName);
	return result;
}

bool SimulationManager::OpenSimulation(const std::string& fileName)
{
	bool result = m_simulation->LoadTimeline(fileName);
	
	if (result)
	{
		m_cameraSystem.Initialize(m_simulation);
	}

	return result;
}
		
void SimulationManager::ToggleCameraTracking()
{
	if (m_cameraSystem.IsTrackingObject())
		m_cameraSystem.StopTrackingObject();
	else if (m_selectedAgentId >= 0)
		m_cameraSystem.StartTrackingObject(m_selectedAgent);
}
		
void SimulationManager::StartCameraTracking()
{
	m_cameraSystem.StartTrackingObject(m_selectedAgent);
}

void SimulationManager::StopCameraTracking()
{
	m_cameraSystem.StopTrackingObject();
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
	
	// Check if our selected agent has died.
	// TODO: make an event queue for simultaion (for births and deaths)
	if (m_selectedAgent != nullptr && m_simulation->
		GetObjectManager()->GetObj(m_selectedAgentId) == nullptr)
	{
		m_selectedAgent = nullptr;
		m_selectedAgentId = 0;
		m_cameraSystem.StopTrackingObject();
	}

	// Update camera system.
	m_cameraSystem.Update();
}
