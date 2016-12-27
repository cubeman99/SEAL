#ifndef _TEST_GL_CANVAS_H_
#define _TEST_GL_CANVAS_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>



class SimulationRenderPanel : public wxGLCanvas
{
public:
    SimulationRenderPanel(wxWindow* parent, int* attribList = NULL);

private:
    void OnPaint(wxPaintEvent& e);
    void Spin(float xSpin, float ySpin);
    void OnKeyDown(wxKeyEvent& e);
    void OnSpinTimer(wxTimerEvent& e);

    // angles of rotation around x and y axes.
    float m_xangle;
    float m_yangle;

    wxTimer m_spinTimer;

    wxDECLARE_EVENT_TABLE();
};


#endif // _TEST_GL_CANVAS_H_