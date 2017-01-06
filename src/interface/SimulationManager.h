#ifndef _SIMULATION_MANAGER_H_
#define _SIMULATION_MANAGER_H_

#include <graphics/OpenGLIncludes.h>
#include <math/Quaternion.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include <interface/CameraSystem.h>


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

	inline bool IsViewWireFrameMode() const { return m_viewWireFrameMode; }
	void SetViewWireFrameMode(bool viewWireFrameMode) { m_viewWireFrameMode = viewWireFrameMode; }
	inline bool IsLightingEnabled() const { return m_viewLighting; }
	void EnableLighting(bool enableLighting) { m_viewLighting = enableLighting; }

	inline void SetSelectedAgent(Agent* agent) { m_selectedAgent = agent; }

	void ToggleCameraTracking();
	void StartCameraTracking();
	void StopCameraTracking();
	void PauseSimulation();
	
	void Update();
	
private:
	Simulation* m_simulation;
	CameraSystem m_cameraSystem;
	bool m_isSimulationPaused;
	Agent* m_selectedAgent;
	bool m_viewWireFrameMode;
	bool m_viewLighting;
};


#endif // _SIMULATION_MANAGER_H_