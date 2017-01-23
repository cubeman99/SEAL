#ifndef _CAMERA_SYSTEM_H_
#define _CAMERA_SYSTEM_H_

#include <graphics/ICamera.h>
#include <math/Ray.h>
#include <math/Vector2f.h>
#include <simulation/Simulation.h>
#include "GlobeCamera.h"
#include "ArcBallCamera.h"

class Agent;


class CameraSystem
{
public:
	CameraSystem();

	void Initialize(Simulation* simulation);

	inline ICamera* GetActiveCamera() const { return m_camera; }
	inline Matrix4f GetViewProjectionMatrix() const { return m_camera->GetViewProjection(); }
	inline bool IsTrackingAgent() const { return m_cameraTracking; }

	Ray GetRay(const Vector2f& screenCoord);

	void SetFieldOfView(float fieldOfView);
	void SetAspectRatio(float aspectRatio);
	void StartTrackingAgent(Agent* agent);
	void StopTrackingAgent();

	void Update();

private:

	bool m_cameraTracking;
	Agent* m_trackedAgent;
	Simulation* m_simulation;
	
	ICamera* m_camera; // The currently-active camera.
	GlobeCamera m_globeCamera;
	ArcBallCamera m_arcBallCamera;
	
	float m_aspectRatio;
	float m_fieldOfView;
	bool m_isProjectionDirty;
};


#endif // _CAMERA_SYSTEM_H_