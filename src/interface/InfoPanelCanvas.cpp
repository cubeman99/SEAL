#include "InfoPanelCanvas.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Agent.h>
#include <simulation/Simulation.h>
#include "SimulationWindow.h"
#include <sstream>
#include <iomanip>


// ----------------------------------------------------------------------------
// Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(InfoPanelCanvas, wxGLCanvas)
    EVT_PAINT(InfoPanelCanvas::OnPaint)
wxEND_EVENT_TABLE()


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

InfoPanelCanvas::InfoPanelCanvas(wxWindow* parent, SimulationWindow* simulationWindow) :
    wxGLCanvas(parent, wxID_ANY, nullptr,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE)
{
	m_simulationWindow = simulationWindow;

	// Force the OpenGL context to be created now.
	wxGetApp().GetGLContext(this);
}

InfoPanelCanvas::~InfoPanelCanvas()
{
	// The resource manager will automatically delete our resources.
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------

SimulationManager* InfoPanelCanvas::GetSimulationManager()
{
	return m_simulationWindow->GetSimulationManager();
}

Simulation* InfoPanelCanvas::GetSimulation()
{
	return m_simulationWindow->GetSimulationManager()->GetSimulation();
}


//-----------------------------------------------------------------------------
// UI Events
//-----------------------------------------------------------------------------

void InfoPanelCanvas::OnPaint(wxPaintEvent& e)
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

	wxGetApp().GetGLContext(this);
	
	// Get the view port size.
    const wxSize clientSize = GetClientSize();
	Vector2f canvasSize;
	canvasSize.x = (float) clientSize.x;
	canvasSize.y = (float) clientSize.y;
	
	Graphics m_graphics;
	m_graphics.SetupCanvas2D(canvasSize.x, canvasSize.y);
	m_graphics.Clear(Color::BLACK);
	
	Simulation* simulation = m_simulationWindow->
		GetSimulationManager()->GetSimulation();
	const SimulationStats& stats = simulation->GetStatistics();
	const SimulationConfig& config = simulation->GetConfig();
	Font* m_font = m_simulationWindow->GetSimulationManager()->
		GetResourceManager()->GetFont("font");
	
	Color colBlue(0, 106, 255);
	std::string degSymbol = "x";
	degSymbol[0] = (char) 248;


	Rect2f bounds(Vector2f::ZERO, canvasSize);

	//-------------------------------------------------------------------------
	// Simulation info panel.

	m_simInfoPanel.SetTitle("Simulation");
	m_simInfoPanel.SetFont(m_font);
	m_simInfoPanel.Clear();
	m_simInfoPanel.AddItem("world age").SetValue(simulation->GetAgeInTicks());
	m_simInfoPanel.AddItem("generation").SetValue(simulation->GetGeneration());
	m_simInfoPanel.AddItem("season age").SetValue(simulation->GetGenerationAge()).InitBar(Color::MAGENTA, 0u, simulation->GetGenerationDuration());
	m_simInfoPanel.AddItem("population size").SetValue((int) stats.populationSize).InitBar(Color::CYAN, config.agent.minPreyAgents, config.agent.maxPreyAgents);
	m_simInfoPanel.AddSeparator();
	m_simInfoPanel.AddItem("total energy").SetValue(stats.totalEnergy);
	m_simInfoPanel.AddItem("avg energy").SetValue(stats.avgEnergy);
	m_simInfoPanel.AddItem("avg energy usage").SetValue(stats.avgEnergyUsage);
	m_simInfoPanel.AddItem("avg fitness").SetValue(stats.avgFitness);
	m_simInfoPanel.AddSeparator();
	m_simInfoPanel.AddItem("avg move amount").SetValue(stats.avgMoveAmount).InitBar(Color::GREEN, 0, 1);
	m_simInfoPanel.AddItem("avg turn amount").SetValue(stats.avgTurnAmount).InitBar(Color::CYAN, 0, 1);
	m_simInfoPanel.Draw(m_graphics, bounds);

	//-------------------------------------------------------------------------
	// Agent info panel.

	Agent* agent = m_simulationWindow->GetSimulationManager()->GetSelectedAgent();
	if (agent != nullptr)
	{
		Genome* genome = agent->GetGenome();

		std::stringstream text;

		// Set title text.
		text << "Agent " << agent->GetId();
		std::string titleText = text.str();

		// Set color value text as a hexcode
		Color col = Color(agent->GetColor());
		text.str("");
		text << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.r
			<< std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.g
			<< std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.b;
		std::string colorText = text.str();
		
		m_agentInfoPanel.SetTitle(titleText);
		m_agentInfoPanel.SetFont(m_font);
		m_agentInfoPanel.Clear();
		m_agentInfoPanel.AddItem("age").SetValue(agent->GetAge()).InitBar(Color::MAGENTA, 0, agent->GetLifeSpan());
		m_agentInfoPanel.AddItem("energy").SetValue(agent->GetEnergy()).InitBar(Color::YELLOW, 0.0f, agent->GetMaxEnergy());
		m_agentInfoPanel.AddItem("energy usage").SetValue(agent->GetEnergyUsage()).SetPrecision(4);
		m_agentInfoPanel.AddItem("fitness").SetValue(agent->GetFitness());
		m_agentInfoPanel.AddSeparator();
		m_agentInfoPanel.AddItem("move speed").SetValue(agent->GetMoveSpeed()).InitBar(Color::GREEN, 0.0f, agent->GetMaxMoveSpeed());
		m_agentInfoPanel.AddItem("turn speed").SetValue(-agent->GetTurnSpeed()).InitBar(Color::CYAN, -agent->GetMaxTurnSpeed(), agent->GetMaxTurnSpeed(), 0.0f);
		m_agentInfoPanel.AddSeparator();
		m_agentInfoPanel.AddItem("color").SetValue(text.str()).InitSolidColor(agent->GetColor());
		m_agentInfoPanel.AddItem("life span").SetValue(agent->GetLifeSpan()).InitBar(Color::MAGENTA, config.genes.minLifeSpan, config.genes.maxLifeSpan);
		m_agentInfoPanel.AddItem("strength").SetValue(agent->GetStrength()).InitBar(Color::RED, config.genes.minStrength, config.genes.maxStrength);
		m_agentInfoPanel.AddItem("num children").SetValue(agent->GetDesiredNumChildren()).InitBar(Color::GREEN, config.genes.minChildren, config.genes.maxChildren);
		m_agentInfoPanel.AddItem("muation rate", "%").SetValue(agent->GetMutationRate() * 100).InitBar(Color::MAGENTA, config.genes.minMutationRate * 100, config.genes.maxMutationRate * 100);
		m_agentInfoPanel.AddItem("crossover points").SetValue(agent->GetNumCrossoverPoints()).InitBar(Color::YELLOW, config.genes.minCrossoverPoints, config.genes.maxCrossoverPoints);
		m_agentInfoPanel.AddItem("field of view", degSymbol).SetValue(Math::ToDegrees(agent->GetFieldOfView())).SetPrecision(0).InitBar(Color::GREEN, Math::ToDegrees(config.genes.minFieldOfView), Math::ToDegrees(config.genes.maxFieldOfView));
		m_agentInfoPanel.AddItem("angle b/w eyes", degSymbol).SetValue(Math::ToDegrees(agent->GetAngleBetweenEyes())).SetPrecision(0).InitBar(Color(255, 128, 0), Math::ToDegrees(config.genes.minAngleBetweenEyes), Math::ToDegrees(config.genes.maxAngleBetweenEyes));
		m_agentInfoPanel.AddItem("sight distance").SetValue(agent->GetMaxViewDistance()).InitBar(Color::CYAN, config.genes.minSightDistance, config.genes.maxSightDistance);
		m_agentInfoPanel.AddItem("resolution red").SetValue(agent->GetSightResolution(0)).InitBar(Color::RED, config.genes.minSightResolution, config.genes.maxSightResolution);
		m_agentInfoPanel.AddItem("resolution green").SetValue(agent->GetSightResolution(1)).InitBar(Color::GREEN, config.genes.minSightResolution, config.genes.maxSightResolution);
		m_agentInfoPanel.AddItem("resolution blue").SetValue(agent->GetSightResolution(2)).InitBar(colBlue, config.genes.minSightResolution, config.genes.maxSightResolution);

		Vector2f panelPos(0, 0);
		panelPos.y += m_simInfoPanel.GetSize().y;
		m_agentInfoPanel.Draw(m_graphics, Rect2f(
			panelPos, canvasSize));
	}


    SwapBuffers();
}

