#include "SimulationManager.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <utilities/Timing.h>
#include <utilities/Logging.h>
#include <simulation/Simulation.h>


// ----------------------------------------------------------------------------
// Constructor & destructor
// ----------------------------------------------------------------------------

SimulationManager::SimulationManager() :
	  m_simulationRenderer(this),
	  m_diagramDrawer(this),
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
	  m_showAxisLines(false),
	  m_maxTicksPerFrame(false),
	  m_debugMode(false),
	  m_ticksPerFrame(1)
{
}

SimulationManager::~SimulationManager()
{
	delete m_simulation;
	m_simulation = nullptr;
}


//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

void SimulationManager::Initialize()
{
	// Initialize sub-systems.
	m_cameraSystem.Initialize();
	m_simulationRenderer.Initialize();
	m_graphManager.Initialize();
	m_diagramDrawer.Initialize();

	// Seed Random with current time.
	// The static Random class should NOT be used for simulation logic.
	// It should be used for graphics and UI stuff.
	Random::SeedTime();

	// Begin a new simulation with default config values.
	// TODO: load default config values from a default config file.
	SimulationConfig config;
	BeginNewSimulation(config);
}
		
void SimulationManager::OnNewSimulation()
{
	// Notify sub-systems of the new simulation.
	m_cameraSystem.OnNewSimulation(m_simulation);
	m_simulationRenderer.OnNewSimulation(m_simulation);
	m_graphManager.OnNewSimulation(m_simulation);

	// Reset viewing state.
	m_debugMode				= false;
	m_selectedAgent			= nullptr;
	m_selectedAgentId		= -1;
	m_isSimulationPaused	= false;
	m_viewWireFrameMode		= false;
	m_viewLighting			= true;
	m_showOctTree			= false;
	m_showOctTreeWireFrame	= false;
	m_showAgentVision		= false;
	m_showAgentBrain		= false;
	m_showInvisibleObjects	= false;
	m_showAxisLines			= false;
	m_maxTicksPerFrame		= false;
	m_ticksPerFrame			= 1;

	// DEBUG: load a test config file.
	SimulationConfig config;
	std::string errorMessage;
	memset((char*) &config, 0, sizeof(SimulationConfig));
	bool success = m_configLoader.LoadConfigFile("../../assets/configs/test_config.txt", config, errorMessage);

	if (!success)
		SEAL_LOG_MSG("%s", errorMessage.c_str());
	else
		SEAL_LOG_MSG("Successfully loaded test config file");
}

//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------

void SimulationManager::BeginNewSimulation(const SimulationConfig& config)
{
	if (m_simulation == nullptr)
		m_simulation = new Simulation();
	
	m_simulation->Initialize(config);

	OnNewSimulation();
}

// Should be called only between ticks in the simulation.
bool SimulationManager::SaveSimulation(const std::string& fileName)
{
	std::ofstream fileOut;
	fileOut.open(fileName, std::ios::out | std::ios::binary);

	if (!fileOut)
	{
		return false; // TODO: Tell user that the file could not be opened for writing
	}

	// Write the simulation data
	if (!m_simulation->WriteSimulation(fileOut))
	{
		fileOut.close();
		return false;
	}

	fileOut.close();
	return true; // TODO: Tell user that the file has been saved succesffully
}

bool SimulationManager::OpenSimulation(const std::string& fileName)
{
	std::ifstream fileIn;
	fileIn.open(fileName, std::ios::in | std::ios::binary);

	if (!fileIn)
	{
		// TODO: Tell user that the file could not be opened for reading
		return false;
	}

	// Read the simulation data
	if (!m_simulation->ReadSimulation(fileIn))
	{
		fileIn.close();
		return false;
	}

	fileIn.close();

	OnNewSimulation();

	// TODO: Tell user that the file has been loaded succesffully
	return true;
}


//-----------------------------------------------------------------------------
// Updates
//-----------------------------------------------------------------------------

void SimulationManager::TickSimulation()
{
	m_simulation->Tick();
}

void SimulationManager::Update()
{
	// Update simulation.
	if (!m_isSimulationPaused)
	{
		if (m_maxTicksPerFrame)
		{
			double startTime = Time::GetTime();
			double elapsedTime = 0.0;
			while (elapsedTime < 0.016667)
			{
				TickSimulation();
				elapsedTime = Time::GetTime() - startTime;
			}
		}
		else
		{
			for (unsigned int i = 0; i < m_ticksPerFrame; ++i)
			{
				TickSimulation();
			}
		}
	}
	
	// Check if our selected agent has died to prevent dangling references.
	// TODO: make an event queue for simultaion (for births and deaths)
	if (m_selectedAgent != nullptr && m_simulation->
		GetObjectManager()->GetObjectById(m_selectedAgentId) == nullptr)
	{
		m_selectedAgent = nullptr;
		m_selectedAgentId = 0;
		m_cameraSystem.StopTrackingObject();
	}

	// Update camera system.
	m_cameraSystem.Update();
}


//-----------------------------------------------------------------------------
// Interface controls
//-----------------------------------------------------------------------------

void SimulationManager::SetSelectedAgent(Agent* agent)
{
	m_selectedAgent = agent;
	if (agent == nullptr)
		m_selectedAgentId = -1;
	else
		m_selectedAgentId = agent->GetId();
}


void SimulationManager::SetCameraTracking(bool cameraTracking)
{
	if (cameraTracking && m_selectedAgent != nullptr)
		m_cameraSystem.StartTrackingObject(m_selectedAgent);
	else
		m_cameraSystem.StopTrackingObject();
}

