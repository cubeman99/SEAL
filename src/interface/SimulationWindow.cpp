#include "SimulationWindow.h"
#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include <sstream>
#include <utilities/Timing.h>


enum
{
    NEW_STEREO_WINDOW = wxID_HIGHEST + 1,

	PLAY_PAUSE_SIMULATION,

	TOGGLE_CAMERA_TRACKING,
	VIEW_WIREFRAME_MODE,
	VIEW_LIGHTING,
	
	DEBUG_SPAWN_AGENTS,
	
	UPDATE_TIMER,
};


wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)
    
	EVT_MENU(wxID_NEW, SimulationWindow::OnNewWindow)
    EVT_MENU(wxID_CLOSE, SimulationWindow::OnClose)
    EVT_MENU(PLAY_PAUSE_SIMULATION, SimulationWindow::OnPlayPauseSimulation)
    EVT_MENU(TOGGLE_CAMERA_TRACKING, SimulationWindow::OnToggleCameraTracking)
    EVT_MENU(DEBUG_SPAWN_AGENTS, SimulationWindow::OnSpawnAgents)
	EVT_MENU(VIEW_WIREFRAME_MODE, SimulationWindow::OnMenuItem)
	EVT_MENU(VIEW_LIGHTING, SimulationWindow::OnMenuItem)
    EVT_MENU(wxID_ABOUT, SimulationWindow::OnMenuItem)

    EVT_TIMER(UPDATE_TIMER, SimulationWindow::OnUpdateTimer)
    EVT_CLOSE(SimulationWindow::OnWindowClose)
	
wxEND_EVENT_TABLE()


SimulationWindow::SimulationWindow() :
	wxFrame(NULL, wxID_ANY, wxT("New Simulation - SEAL")),
	m_updateTimer(this, UPDATE_TIMER)
{
    m_simulationPanel = new SimulationRenderPanel(this, NULL);

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenuBar* menuBar = new wxMenuBar;
    SetMenuBar(menuBar);

	// FILE
    wxMenu* menuFile = new wxMenu;
    menuBar->Append(menuFile, wxT("&File"));
    menuFile->Append(wxID_NEW);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE, "&Close\tCTRL+W");
	
	// SIMULATION
    wxMenu* menuSimulation = new wxMenu;
    menuBar->Append(menuSimulation, wxT("&Simulation"));
    menuSimulation->Append(PLAY_PAUSE_SIMULATION, "&Play/Pause Simulation\tP");

	// VIEW
    wxMenu* menuView = new wxMenu;
    menuBar->Append(menuView, wxT("&View"));
    menuView->AppendCheckItem(TOGGLE_CAMERA_TRACKING, "Toggle Camera &Tracking\tT");
    menuView->AppendCheckItem(VIEW_WIREFRAME_MODE, "&Wireframe mode\tW");
    menuView->AppendCheckItem(VIEW_LIGHTING, "&Lighting\tL")->Check(true);

	// DEBUG
    wxMenu* menuDebug = new wxMenu;
    menuBar->Append(menuDebug, wxT("&Debug"));
    menuDebug->Append(DEBUG_SPAWN_AGENTS, "&Spawn Agents\tG");

	// HELP
    wxMenu* menuHelp = new wxMenu;
    menuBar->Append(menuHelp, wxT("&Help"));
    menuHelp->Append(wxID_ABOUT);

	// Make a status bar
    CreateStatusBar(3);
	SetStatusText("Hello");

    SetClientSize(800, 600);
    Show();

	//m_updateTimer.Start(17);
	m_updateTimer.Start(8);
	m_frameCounter = 0;
	double lastFrameTimeStamp = Time::GetTime();

	m_simulationManager.Initialize();
}

void SimulationWindow::OnClose(wxCommandEvent& e)
{
    Close(true); // true is to force the frame to close
}

void SimulationWindow::OnNewWindow(wxCommandEvent& e)
{
    new SimulationWindow();
}

void SimulationWindow::OnPlayPauseSimulation(wxCommandEvent& e)
{
	m_simulationManager.PauseSimulation();
}

void SimulationWindow::OnToggleCameraTracking(wxCommandEvent& e)
{
	m_simulationManager.ToggleCameraTracking();
}

void SimulationWindow::OnSpawnAgents(wxCommandEvent& e)
{
	for (int i = 0; i < 10; i++)
		m_simulationManager.GetSimulation()->GetAgentSystem()->SpawnAgent();
}

void SimulationWindow::OnMenuItem(wxCommandEvent& e)
{
	switch (e.GetId())
	{
	case VIEW_WIREFRAME_MODE:
		m_simulationManager.SetViewWireFrameMode(
			!m_simulationManager.IsViewWireFrameMode());
		break;
	case VIEW_LIGHTING:
		m_simulationManager.EnableLighting(
			!m_simulationManager.IsLightingEnabled());
		break;
	case wxID_ABOUT:
	{
		wxMessageBox("SEAL\nSimulation of Evolutionary Artificial Life.\n\nBy David Jordan & Ben Russel (2017)",
			"About SEAL", wxICON_INFORMATION);
		break;
	}
	}
}

void SimulationWindow::OnUpdateTimer(wxTimerEvent& e)
{
	m_simulationManager.Update();
	
	double timeStamp = Time::GetTime();
	m_frameCounter++;

	if (timeStamp > m_lastFrameTimeStamp + 1.0f)
	{
		m_lastFrameTimeStamp = timeStamp;
		m_fps = m_frameCounter;
		m_frameCounter = 0;
	}

	int numAgents = m_simulationManager.GetSimulation()->GetAgentSystem()->GetNumAgents();
	std::stringstream ss;
	ss << numAgents << " agents";
	SetStatusText(ss.str(), 1);
	ss.str("");
	ss << (int) (m_fps + 0.5f) << " fps";
	SetStatusText(ss.str(), 2);
	
	m_simulationPanel->Refresh(true);
}

void SimulationWindow::OnWindowClose(wxCloseEvent& e)
{
	m_updateTimer.Stop();
    Destroy(); // true is to force the frame to close
}
