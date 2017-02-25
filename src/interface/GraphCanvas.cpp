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
	graphBox.Inset(4, 0);
	SimulationManager* simulationManager = m_simulationWindow->GetSimulationManager();

	GraphInfo* graph = simulationManager->GetGraphManager()->GetGraph(m_graphIndex);

	float labelHeight = 32;

	Font* font = simulationManager->GetDiagramDrawer()->GetFont();
	
	//-----------------------------------------------------------------------------
	// Draw herbivore graph.

	graphBox.position.y += labelHeight * 0.5f;
	m_graphics.DrawString(font, " Herbivores:", graphBox.position, Color::GREEN, TextAlign::MIDDLE_LEFT);
	graphBox.position.y += labelHeight * 0.5f;

	graph->SetSpecies(Species::SPECIES_HERBIVORE);
	simulationManager->GetDiagramDrawer()->DrawGraph(m_graphics, *graph, graphBox);
	graphBox.position.y += graphBox.size.y;
	
	//-----------------------------------------------------------------------------
	// Draw the carnivore graph.

	graphBox.position.y += labelHeight * 0.5f;
	m_graphics.DrawString(font, " Carnivores:", graphBox.position, Color::RED, TextAlign::MIDDLE_LEFT);
	graphBox.position.y += labelHeight * 0.5f;

	graph->SetSpecies(Species::SPECIES_CARNIVORE);
	simulationManager->GetDiagramDrawer()->DrawGraph(m_graphics, *graph, graphBox);
	graphBox.position.y += graphBox.size.y;
	
	//-----------------------------------------------------------------------------
	// Draw the two species graphs combined.

	GraphInfo combinedGraphs[2];
	combinedGraphs[0] = *graph;
	combinedGraphs[0].SetSpecies(SPECIES_HERBIVORE);
	combinedGraphs[0].SetColor(Color::GREEN);
	combinedGraphs[1] = *graph;
	combinedGraphs[1].SetSpecies(SPECIES_CARNIVORE);
	combinedGraphs[1].SetColor(Color::RED);

	graphBox.position.y += labelHeight * 0.5f;
	m_graphics.DrawString(font, " Combined:", graphBox.position, Color::YELLOW, TextAlign::MIDDLE_LEFT);
	graphBox.position.y += labelHeight * 0.5f;

	graphBox.size.y *= 2.0f;
	graph->SetSpecies(Species::SPECIES_CARNIVORE);
	simulationManager->GetDiagramDrawer()->DrawGraphs(m_graphics, combinedGraphs, 2, graphBox);
	graphBox.position.y += graphBox.size.y;


    SwapBuffers();
}

