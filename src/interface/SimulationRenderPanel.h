#ifndef _TEST_GL_CANVAS_H_
#define _TEST_GL_CANVAS_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <simulation/World.h>
#include <math/Quaternion.h>


struct Agent
{
	Vector3f position;
	Quaternion orientation;
};


class SimulationRenderPanel : public wxGLCanvas
{
public:
    SimulationRenderPanel(wxWindow* parent, int* attribList = NULL);

private:
    void OnPaint(wxPaintEvent& e);
    void Spin(float xSpin, float ySpin);
    void OnKeyDown(wxKeyEvent& e);
	void OnMouseDown(wxMouseEvent& e);
	void OnMouseMotion(wxMouseEvent& e);
	void OnMouseWheel(wxMouseEvent& e);
    void OnSpinTimer(wxTimerEvent& e);
    void OnSize(wxSizeEvent& e);

    // angles of rotation around x and y axes.
    float m_xangle;
    float m_yangle;
	float m_scale;

    wxTimer m_spinTimer;
	World m_world;
	Quaternion m_cameraRotation;

	std::vector<Agent> m_agents;

    wxDECLARE_EVENT_TABLE();
};


#endif // _TEST_GL_CANVAS_H_