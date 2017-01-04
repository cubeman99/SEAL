#ifndef _SIMULATION_MANAGER_H_
#define _SIMULATION_MANAGER_H_

#include <graphics/OpenGLIncludes.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include <math/Quaternion.h>
#include <interface/ICamera.h>
#include <interface/GlobeCamera.h>
#include <interface/ArcBallCamera.h>
#include <interface/CameraSystem.h>


class SimulationManager
{
public:
    SimulationManager();
	~SimulationManager();

	inline Simulation* GetSimulation() { return m_simulation; }
	inline bool IsSimulationPuased() const { return m_isSimulationPaused; }
	inline Agent* GetSelectedAgent() const { return m_selectedAgent; }
	inline ICamera* GetActiveCamera() const { return m_cameraSystem.GetActiveCamera(); }
	inline CameraSystem* GetCameraSystem() { return &m_cameraSystem; }


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
};


#endif // _SIMULATION_MANAGER_H_