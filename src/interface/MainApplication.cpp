#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include "SimulationWindow.h"


IMPLEMENT_APP(MainApplication)


MainApplication::MainApplication() :
	m_glContext(nullptr)
{
}

bool MainApplication::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    new SimulationWindow();

    return true;
}

int MainApplication::OnExit()
{
    delete m_glContext;
	m_glContext = nullptr;

    return wxApp::OnExit();
}

int MainApplication::OnRun()
{
    // generate an initial idle event to start things
    //wxIdleEvent event;
    //event.SetEventObject(&frame->getPanel());
    //frame->getPanel().AddPendingEvent(event);

    return wxApp::OnRun();
}

OpenGLContext& MainApplication::GetGLContext(wxGLCanvas* canvas)
{
    if (m_glContext == NULL)
    {
        // Create the OpenGL context for the first window which needs it:
        // subsequently created windows will all share the same context.
        m_glContext = new OpenGLContext(canvas);
    }

    m_glContext->SetCurrent(*canvas);
    return *m_glContext;
}

