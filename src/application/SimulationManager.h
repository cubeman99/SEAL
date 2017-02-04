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
    SimulationManager();
	~SimulationManager();

	void Initialize();

	inline Simulation* GetSimulation() { return m_simulation; }
	inline bool IsSimulationPuased() const { return m_isSimulationPaused; }
	inline Agent* GetSelectedAgent() const { return m_selectedAgent; }
	inline ICamera* GetActiveCamera() const { return m_cameraSystem.GetActiveCamera(); }
	inline CameraSystem* GetCameraSystem() { return &m_cameraSystem; }
	inline SimulationRenderer* GetSimulationRenderer() { return &m_simulationRenderer; }

	// Render options
	// TODO: have some sort of enum for view options like these.

	inline void SetViewWireFrameMode(bool viewWireFrameMode) { m_viewWireFrameMode = viewWireFrameMode; }
	inline void EnableLighting(bool enableLighting) { m_viewLighting = enableLighting; }
	inline void SetShowOctTree(bool showOctTree) { m_showOctTree = showOctTree; }
	inline void SetShowOctTreeWireFrame(bool showOctTreeWireFrame) { m_showOctTreeWireFrame = showOctTreeWireFrame; }
	inline void SetShowAgentVision(bool showAgentVision) { m_showAgentVision = showAgentVision; }

	inline bool IsViewWireFrameMode() const { return m_viewWireFrameMode; }
	inline bool IsLightingEnabled() const { return m_viewLighting; }
	inline bool GetShowOctTree() const { return m_showOctTree; }
	inline bool GetShowOctTreeWireFrame() const { return m_showOctTreeWireFrame; }
	inline bool GetShowAgentVision() const { return m_showAgentVision; }

	inline void SetSelectedAgent(Agent* agent) { m_selectedAgent = agent; }

	void ToggleCameraTracking();
	void StartCameraTracking();
	void StopCameraTracking();
	void PauseSimulation();
	
	void Update();
	
private:
	SimulationRenderer m_simulationRenderer;
	Simulation* m_simulation;
	CameraSystem m_cameraSystem;
	bool m_isSimulationPaused;
	Agent* m_selectedAgent;

	// Render options.
	bool m_viewWireFrameMode;
	bool m_viewLighting;
	bool m_showOctTree;
	bool m_showOctTreeWireFrame;
	bool m_showAgentVision;
};


#endif // _SIMULATION_MANAGER_H_