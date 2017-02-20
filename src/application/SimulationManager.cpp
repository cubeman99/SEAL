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
	m_simulation = nullptr;
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

//-----------------------------------------------------------------------------
// Saving & loading
//-----------------------------------------------------------------------------

// Should be called only between ticks in the simulation.
bool SimulationManager::SaveSimulation(const std::string& fileName)
{
	std::ofstream fileOut;
	fileOut.open(fileName, std::ios::out | std::ios::binary);

	if (!fileOut)
	{
		// TODO: Tell user that the file could not be opened for writing
		return false;
	}

	// Write the simulation data
	m_simulation->WriteSimulation(fileOut);

	// Write the number of objects
	ObjectManager* objManager = m_simulation->GetObjectManager();
	unsigned int numObjects = objManager->GetNumObjects();
	fileOut.write((char*)&numObjects, sizeof(unsigned int));

	// Write the object data
	for (unsigned int i = 0; i < objManager->GetNumObjects(); ++i)
	{
		objManager->GetObjByIndex(i)->Write(fileOut);
	}

	fileOut.close();

	// TODO: Tell user that the file has been saved succesffully

	return true;
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

	// Clear out the current objects
	ObjectManager* objManager = m_simulation->GetObjectManager();
	objManager->ClearObjects();

	// Read the simulation data
	m_simulation->ReadSimulation(fileIn);

	// Get number of objects
	unsigned int numObjects;
	fileIn.read((char*)&numObjects, sizeof(unsigned int));

	// Read and create objects
	bool objectCreationGoingWell = true;
	for (unsigned int i = 0; i < numObjects && objectCreationGoingWell; ++i)
	{
		objectCreationGoingWell = objManager->SpawnObjectSerialized(fileIn);
	}

	if (!objectCreationGoingWell)
	{
		// TODO: Tell user that the file was corrupt like our government
		objManager->ClearObjects();
		return false;
	}

	fileIn.close();

	// TODO: Tell user that the file has been loaded succesffully

	return true;
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
