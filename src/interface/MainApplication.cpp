#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include "SimulationWindow.h"


IMPLEMENT_APP(MainApplication)


MainApplication::MainApplication()
{
	m_glContext = NULL;
}

bool MainApplication::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new SimulationWindow();

    return true;
}

int MainApplication::OnExit()
{
    delete m_glContext;

    return wxApp::OnExit();
}

OpenGLContext& MainApplication::GetGLContext(wxGLCanvas* canvas)
{
    OpenGLContext* glContext;
    
    if (m_glContext == NULL)
    {
        // Create the OpenGL context for the first window which needs it:
        // subsequently created windows will all share the same context.
        m_glContext = new OpenGLContext(canvas);
    }

    glContext = m_glContext;
    glContext->SetCurrent(*canvas);
    return *glContext;
}

