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

GraphCanvas::GraphCanvas(wxWindow* parent, SimulationWindow* simulationWindow) :
    wxGLCanvas(parent, wxID_ANY, nullptr,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE),
	m_graphIndex(0)
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
	Graphics m_graphics;
	m_graphics.SetupCanvas2D(canvasSize.x, canvasSize.y);
	m_graphics.Clear(Color::BLACK);

	
	Vector2f graphSize(280, 84);
	Rect2f graphBox(0.0f, 0.0f, canvasSize.x, 120);
	SimulationManager* simulationManager = m_simulationWindow->GetSimulationManager();

	GraphInfo* graph = simulationManager->GetGraphManager()->GetGraph(m_graphIndex);
	simulationManager->GetDiagramDrawer()->DrawGraph(m_graphics, *graph, graphBox);

    SwapBuffers();
}

