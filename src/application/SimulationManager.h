#ifndef _SIMULATION_MANAGER_H_
#define _SIMULATION_MANAGER_H_

#include <math/Quaternion.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include "CameraSystem.h"
#include "SimulationRenderer.h"


class SimulationManager
{
public:
	// Constructor & destructor
    SimulationManager();
	~SimulationManager();

	// Initialization
	void Initialize();
	void OnNewSimulation();

	// Simulation
	void BeginNewSimulation(const SimulationConfig& config);
	bool SaveSimulation(const std::string& fileName);
	bool OpenSimulation(const std::string& fileName);
	
	// Updates
	void TickSimulation();
	void Update();

	// Interface controls
	void SetSelectedAgent(Agent* agent);
	void SetCameraTracking(bool cameraTracking);

	// Getters
	inline Simulation* GetSimulation() { return m_simulation; }
	inline ICamera* GetActiveCamera() const { return m_cameraSystem.GetActiveCamera(); }
	inline CameraSystem* GetCameraSystem() { return &m_cameraSystem; }
	inline SimulationRenderer* GetSimulationRenderer() { return &m_simulationRenderer; }
	inline bool IsSimulationPaused() const { return m_isSimulationPaused; }
	inline Agent* GetSelectedAgent() { return m_selectedAgent; }

	// Setters
	inline void SetSimulationPaused(bool isSimulationPaused) { m_isSimulationPaused = isSimulationPaused; }
	inline void SetTicksFerFrame(unsigned int ticksPerFrame) { m_ticksPerFrame = ticksPerFrame; }
	inline void SetMaxTicksPerFrame(bool maxTicksPerFrame) { m_maxTicksPerFrame = maxTicksPerFrame; }

	// Render options
	// TODO: have some sort of enum for view options like these.

	inline void SetViewWireFrameMode(bool viewWireFrameMode) { m_viewWireFrameMode = viewWireFrameMode; }
	inline void EnableLighting(bool enableLighting) { m_viewLighting = enableLighting; }
	inline void SetShowOctTree(bool showOctTree) { m_showOctTree = showOctTree; }
	inline void SetShowOctTreeWireFrame(bool showOctTreeWireFrame) { m_showOctTreeWireFrame = showOctTreeWireFrame; }
	inline void SetShowAgentVision(bool showAgentVision) { m_showAgentVision = showAgentVision; }
	inline void SetShowAgentBrain(bool showAgentBrain) { m_showAgentBrain = showAgentBrain; }
	inline void SetShowInvisibleObjects(bool showInvisibleObjects) { m_showInvisibleObjects = showInvisibleObjects; }
	inline void SetShowAxisLines(bool showAxisLines) { m_showAxisLines = showAxisLines; }

	inline bool IsViewWireFrameMode() const { return m_viewWireFrameMode; }
	inline bool IsLightingEnabled() const { return m_viewLighting; }
	inline bool GetShowOctTree() const { return m_showOctTree; }
	inline bool GetShowOctTreeWireFrame() const { return m_showOctTreeWireFrame; }
	inline bool GetShowAgentVision() const { return m_showAgentVision; }
	inline bool GetShowAgentBrain() const { return m_showAgentBrain; }
	inline bool GetShowInvisibleObjects() const { return m_showInvisibleObjects; }
	inline bool GetShowAxisLines() const { return m_showAxisLines; }
	

private:
	SimulationRenderer m_simulationRenderer;
	Simulation*		m_simulation;
	CameraSystem	m_cameraSystem;
	int				m_selectedAgentId;
	Agent*			m_selectedAgent;

	bool			m_isSimulationPaused;
	unsigned int	m_ticksPerFrame;
	bool			m_maxTicksPerFrame;
	
	// Render options.
	bool m_viewWireFrameMode;
	bool m_viewLighting;
	bool m_showOctTree;
	bool m_showOctTreeWireFrame;
	bool m_showAgentVision;
	bool m_showAgentBrain;
	bool m_showInvisibleObjects;
	bool m_showAxisLines;
};


#endif // _SIMULATION_MANAGER_H_