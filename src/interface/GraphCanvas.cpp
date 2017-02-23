#include "GraphCanvas.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Agent.h>
#include <simulation/Simulation.h>
#include "SimulationWindow.h"

// ----------------------------------------------------------------------------
// Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(GraphCanvas, wxGLCanvas)
    EVT_PAINT(GraphCanvas::OnPaint)
wxEND_EVENT_TABLE()


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

GraphCanvas::GraphCanvas(wxWindow* parent, SimulationWindow* simulationWindow)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : wxGLCanvas(parent, wxID_ANY, nullptr,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE)
{
	m_simulationWindow = simulationWindow;

	// Force the OpenGL context to be created now.
	wxGetApp().GetGLContext(this);
}

GraphCanvas::~GraphCanvas()
{
	// The resource manager will automatically delete our resources.
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------

SimulationManager* GraphCanvas::GetSimulationManager()
{
	return m_simulationWindow->GetSimulationManager();
}

Simulation* GraphCanvas::GetSimulation()
{
	return m_simulationWindow->GetSimulationManager()->GetSimulation();
}


//-----------------------------------------------------------------------------
// UI Events
//-----------------------------------------------------------------------------

void GraphCanvas::OnPaint(wxPaintEvent& e)
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

	wxGetApp().GetGLContext(this);
	
	// Get the view port size.
    const wxSize clientSize = GetClientSize();
	Vector2f canvasSize;
	canvasSize.x = (float) clientSize.x;
	canvasSize.y = (float) clientSize.y;
	
	// Switch to orthographic mode to render the HUD.
	Matrix4f orthographic = Matrix4f::CreateOrthographic(
		0.0f, canvasSize.x, canvasSize.y, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(orthographic.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
	orthographic = Matrix4f::CreateOrthographic(0.5f,
		canvasSize.x + 0.5f, canvasSize.y + 0.5f, 0.5f, -1.0f, 1.0f);
	Graphics m_graphics;
	m_graphics.SetProjection(orthographic);
	m_graphics.SetCanvasSize(canvasSize.x, canvasSize.y);
	m_graphics.SetViewportToCanvas();

	m_graphics.Clear(Color::BLACK);

	//m_graphics.FillRect(32, 48, 128, 64, Color::GREEN);


	Rect2f box(Vector2f::ZERO, canvasSize);
	box.Inset(32, 32);
	m_graphics.DrawRect(box, Color::MAGENTA);
	
    SwapBuffers();
}

