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
	SHOW_OCT_TREE_ON_SURFACE,
	SHOW_OCT_TREE_WIRE_FRAME,
	SHOW_AGENT_VISION,
	SHOW_AGENT_BRAIN,
	SHOW_INVISIBLE_OBJECTS,

	DEBUG_SPAWN_AGENTS,
	DEBUG_DELETE_AGENT,

	UPDATE_TIMER,
};


wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)
    
	EVT_MENU(wxID_NEW, SimulationWindow::OnNewWindow)
    EVT_MENU(wxID_OPEN, SimulationWindow::OnOpenSimulation)
    EVT_MENU(wxID_SAVE, SimulationWindow::OnSaveSimulation)
    EVT_MENU(wxID_CLOSE, SimulationWindow::OnClose)

    EVT_MENU(PLAY_PAUSE_SIMULATION, SimulationWindow::OnPlayPauseSimulation)

    EVT_MENU(TOGGLE_CAMERA_TRACKING, SimulationWindow::OnToggleCameraTracking)
	EVT_MENU(VIEW_WIREFRAME_MODE, SimulationWindow::OnMenuItem)
	EVT_MENU(VIEW_LIGHTING, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_OCT_TREE_ON_SURFACE, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_OCT_TREE_WIRE_FRAME, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_AGENT_VISION, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_AGENT_BRAIN, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_INVISIBLE_OBJECTS, SimulationWindow::OnMenuItem)
	
    EVT_MENU(DEBUG_SPAWN_AGENTS, SimulationWindow::OnSpawnAgents)
    EVT_MENU(DEBUG_DELETE_AGENT, SimulationWindow::OnMenuItem)

    EVT_MENU(wxID_ABOUT, SimulationWindow::OnMenuItem)
	
    EVT_IDLE(SimulationWindow::OnIdle)
    //EVT_TIMER(UPDATE_TIMER, SimulationWindow::OnUpdateTimer)
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
    menuFile->Append(wxID_NEW, "&New Simulation...\tCtrl+N");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_SAVE, "&Save Simulation Timeline...\tCtrl+S");
    menuFile->Append(wxID_OPEN, "&Open Simulation Timeline...\tCtrl+O");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE, "&Close\tCtrl+W");
	
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
    menuView->AppendCheckItem(SHOW_OCT_TREE_ON_SURFACE, "Show OctTree on &Surface\tO")->Check(false);
    menuView->AppendCheckItem(SHOW_OCT_TREE_WIRE_FRAME, "Show Oct&Tree Wireframe\tShift+O")->Check(false);
    menuView->AppendCheckItem(SHOW_AGENT_VISION, "Show Agent &Vision\tA")->Check(false);
    menuView->AppendCheckItem(SHOW_AGENT_BRAIN, "Show Agent &Brain\tB")->Check(false);
    menuView->AppendCheckItem(SHOW_INVISIBLE_OBJECTS, "Show &Invisible Objects\tI")->Check(false);

	// DEBUG
    wxMenu* menuDebug = new wxMenu;
    menuBar->Append(menuDebug, wxT("&Debug"));
    menuDebug->Append(DEBUG_SPAWN_AGENTS, "&Spawn Agents\tG");
    menuDebug->Append(DEBUG_DELETE_AGENT, "&Delete Selected Agent\tDelete");

	// HELP
    wxMenu* menuHelp = new wxMenu;
    menuBar->Append(menuHelp, wxT("&Help"));
    menuHelp->Append(wxID_ABOUT);

	// Make a status bar
    CreateStatusBar(5);
	SetStatusText("Hello");

    SetClientSize(800, 600);
    Show();

	//m_updateTimer.Start(17);
	//m_updateTimer.Start(8);
	//m_updateTimer.Start(2);
	m_frameCounter = 0;
	double lastFrameTimeStamp = Time::GetTime();

	// Initialize a new simulation.
	m_simulationManager.Initialize();

	m_controlledAgentId = -1;
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
	{
		m_simulationManager.GetSimulation()->
			GetObjectManager()->SpawnObjectRandom<Agent>();
	}
}

void SimulationWindow::OnMenuItem(wxCommandEvent& e)
{
	switch (e.GetId())
	{
	case VIEW_WIREFRAME_MODE:
		m_simulationManager.SetViewWireFrameMode(e.IsChecked());
		break;
	case VIEW_LIGHTING:
		m_simulationManager.EnableLighting(e.IsChecked());
		break;
	case SHOW_OCT_TREE_ON_SURFACE:
		m_simulationManager.SetShowOctTree(e.IsChecked());
		break;
	case SHOW_OCT_TREE_WIRE_FRAME:
		m_simulationManager.SetShowOctTreeWireFrame(e.IsChecked());
		break;
	case SHOW_AGENT_VISION:
		m_simulationManager.SetShowAgentVision(e.IsChecked());
		break;
	case SHOW_AGENT_BRAIN:
		m_simulationManager.SetShowAgentBrain(e.IsChecked());
		break;
	case SHOW_INVISIBLE_OBJECTS:
		m_simulationManager.SetShowInvisibleObjects(e.IsChecked());
		break;
	case DEBUG_DELETE_AGENT:
		if (m_simulationManager.GetSelectedAgent() != nullptr)
			m_simulationManager.GetSelectedAgent()->Destroy();
		break;
	case wxID_ABOUT:
	{
		wxMessageBox("SEAL\nSimulation of Evolutionary Artificial Life.\n\nBy David Jordan & Ben Russel (2017)",
			"About SEAL", wxICON_INFORMATION);
		break;
	}
	}
}


void SimulationWindow::OnIdle(wxIdleEvent& e)
{
	double startTime = Time::GetTime(); // time the update.
	
	// Update debug agent move keyboard controls.
	Agent* agent = m_simulationManager.GetSelectedAgent();
	if (m_controlledAgentId >= 0 && 
		(agent == nullptr || agent->GetId() != m_controlledAgentId))
	{
		Agent* controlledAgent = (Agent*) m_simulationManager
			.GetSimulation()->GetObjectManager()->GetObj(m_controlledAgentId);
		if (controlledAgent != nullptr)
			controlledAgent->SetManualOverride(false);
		if (agent != nullptr)
			m_controlledAgentId = agent->GetId();
		else
			m_controlledAgentId = -1;
	}
	if (agent != nullptr)
	{
		int moveAmount = 0;
		int turnAmount = 0;

		// Update movement controls (arrow keys).
		if (wxGetKeyState(WXK_LEFT))
			turnAmount++;
		if (wxGetKeyState(WXK_RIGHT))
			turnAmount--;
		if (wxGetKeyState(WXK_UP))
			moveAmount++;
		if (wxGetKeyState(WXK_DOWN))
			moveAmount--;

		// Disable wandering if the agent was moved manually.
		if (agent->GetManualOverride() || moveAmount != 0 || turnAmount != 0)
		{
			agent->SetTurnSpeed(turnAmount * agent->GetMaxTurnSpeed() * 0.5f);
			agent->SetMoveSpeed(moveAmount * agent->GetMaxMoveSpeed());
			agent->SetManualOverride(true);
		}
	}

	// Update the simulation.
	m_simulationManager.Update();
	
	// Update the FPS counter.
	double timeStamp = Time::GetTime();
	m_frameCounter++;
	if (timeStamp > m_lastFrameTimeStamp + 1.0f)
	{
		m_lastFrameTimeStamp = timeStamp;
		m_fps = m_frameCounter;
		m_frameCounter = 0;
	}

	// Number of agents.
	//int numAgents = m_simulationManager.GetSimulation()->GetAgentSystem()->GetNumAgents();
	int numObjects = m_simulationManager.GetSimulation()->GetObjectManager()->GetNumObjects();
	std::stringstream ss;
	//ss << numAgents << " agents";
	ss << numObjects << " objects";
	SetStatusText(ss.str(), 1);
	
	// FPS.
	ss.str("");
	ss << (int) (m_fps + 0.5f) << " fps";
	SetStatusText(ss.str(), 2);
	
	// Update time in milliseconds
	double endTime = Time::GetTime();
	double updateTimeMs = (endTime - startTime) * 1000.0;
	double renderTimeMs = m_simulationManager.GetSimulationRenderer()->GetAverageRenderTime() * 1000.0;
	ss.str("");
	ss.setf(std::ios::fixed, std::ios::floatfield);
	ss.precision(2);
	ss << "Update time: " << updateTimeMs << " ms";
	ss << " (total: " << (updateTimeMs + renderTimeMs) << " ms";
	SetStatusText(ss.str(), 3);
	
	// Render time in milliseconds.
	ss.str("");
	ss.setf(std::ios::fixed, std::ios::floatfield);
	ss.precision(2);
	ss << "Render time: " << renderTimeMs << " ms";
	SetStatusText(ss.str(), 4);
	
	// Tell the simulation panel to render.
	m_simulationPanel->Refresh(false);
}

void SimulationWindow::OnUpdateTimer(wxTimerEvent& e)
{
	// Update timer event changed to idle event.
}

void SimulationWindow::OnWindowClose(wxCloseEvent& e)
{
	m_updateTimer.Stop();
    Destroy(); // true is to force the frame to close
}


void SimulationWindow::OnOpenSimulation(wxCommandEvent& e)
{
	wxFileDialog* openDialog = new wxFileDialog(this,
		"Choose a file to open", wxEmptyString, wxEmptyString, 
		"Simulation files (*.bin)|*.bin",
		wxFD_OPEN, wxDefaultPosition);
 
	if (openDialog->ShowModal() == wxID_OK)
	{
		std::string path = (std::string) openDialog->GetPath();
		
		if (m_simulationManager.OpenSimulation(path))
		{
			// Update the window title to the opened simulation file.
			SetTitle(openDialog->GetFilename() << wxString(" - SEAL"));
		}
		else
		{
			wxMessageBox(wxString("Error loading simulation ") <<
				openDialog->GetPath(), "Open Simulation", wxICON_WARNING);
		}
	}
 
	openDialog->Destroy();
}

void SimulationWindow::OnSaveSimulation(wxCommandEvent& e)
{
	wxFileDialog* saveDialog = new wxFileDialog(this,
		"Choose a file to save", wxEmptyString, wxEmptyString, 
		"Simulation files (*.bin)|*.bin",
		wxFD_SAVE, wxDefaultPosition);
 
	if (saveDialog->ShowModal() == wxID_OK)
	{
		std::string path = (std::string) saveDialog->GetPath();
		
		if (m_simulationManager.SaveSimulation(path))
		{
			// success!
		}
		else
		{
			wxMessageBox(wxString("Error saving simulation to ") <<
				saveDialog->GetPath(), "Save Simulation", wxICON_WARNING);
		}
	}
 
	saveDialog->Destroy();
}

