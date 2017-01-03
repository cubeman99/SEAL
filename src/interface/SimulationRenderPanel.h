#ifndef _TEST_GL_CANVAS_H_
#define _TEST_GL_CANVAS_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include <math/Quaternion.h>
#include <interface/ICamera.h>
#include <interface/GlobeCamera.h>
#include <interface/ArcBallCamera.h>


// This is the panel where the simulation is drawn and can be interacted with.
class SimulationRenderPanel : public wxGLCanvas
{
public:
    SimulationRenderPanel(Simulation* simulation,
		wxWindow* parent, int* attribList = NULL);
		
	void OnWindowClose();

private:
	wxDECLARE_EVENT_TABLE();

    void OnKeyDown(wxKeyEvent& e);
	void OnMouseDown(wxMouseEvent& e);
	void OnMouseMotion(wxMouseEvent& e);
	void OnMouseWheel(wxMouseEvent& e);
    void OnTickTimer(wxTimerEvent& e);
    void OnPaint(wxPaintEvent& e);

	float m_scale;
    wxTimer m_tickTimer;
	Simulation* m_simulation;
	Quaternion m_cameraRotation;

	ICamera* m_camera; // The currently-active camera.
	GlobeCamera m_globeCamera;
	ArcBallCamera m_arcBallCamera;

	Agent* m_followAgent;
};


#endif // _TEST_GL_CANVAS_H_