#include "SimulationWindow.h"
#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include <sstream>
#include <utilities/Timing.h>


//-----------------------------------------------------------------------------
// Event enumeration
//-----------------------------------------------------------------------------

enum
{
    NEW_STEREO_WINDOW = wxID_HIGHEST + 1,

	// File
	// (using default wxIDs)

	// Simulation
	PLAY_PAUSE_SIMULATION,
	SIMULATION_TICK_ONCE,
	SIMULATION_SPEED_100,
	SIMULATION_SPEED_200,
	SIMULATION_SPEED_400,
	SIMULATION_SPEED_MAX,

	// View
	TOGGLE_CAMERA_TRACKING,
	VIEW_WIREFRAME_MODE,
	VIEW_LIGHTING,
	SHOW_OCT_TREE_ON_SURFACE,
	SHOW_OCT_TREE_WIRE_FRAME,
	SHOW_AGENT_VISION,
	SHOW_AGENT_BRAIN,
	SHOW_INVISIBLE_OBJECTS,

	// Debug
	DEBUG_SPAWN_AGENTS,
	DEBUG_DELETE_AGENT,

	// Help
	// (using default wxIDs)

	UPDATE_TIMER,
};


//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)

	EVT_UPDATE_UI(SIMULATION_TICK_ONCE, SimulationWindow::OnUpdateMenuItem)
    EVT_IDLE(SimulationWindow::OnIdle)
    EVT_CLOSE(SimulationWindow::OnWindowClose)

	// File
	EVT_MENU(wxID_NEW, SimulationWindow::OnNewWindow)
    EVT_MENU(wxID_OPEN, SimulationWindow::OnOpenSimulation)
    EVT_MENU(wxID_SAVE, SimulationWindow::OnSaveSimulation)
    EVT_MENU(wxID_CLOSE, SimulationWindow::OnClose)

	// Simulation
    EVT_MENU(PLAY_PAUSE_SIMULATION, SimulationWindow::OnMenuItem)
    EVT_MENU(SIMULATION_TICK_ONCE, SimulationWindow::OnMenuItem)
    EVT_MENU(SIMULATION_SPEED_100, SimulationWindow::OnMenuItem)
    EVT_MENU(SIMULATION_SPEED_200, SimulationWindow::OnMenuItem)
    EVT_MENU(SIMULATION_SPEED_400, SimulationWindow::OnMenuItem)
    EVT_MENU(SIMULATION_SPEED_MAX, SimulationWindow::OnMenuItem)

	// View
    EVT_MENU(TOGGLE_CAMERA_TRACKING, SimulationWindow::OnMenuItem)
	EVT_MENU(VIEW_WIREFRAME_MODE, SimulationWindow::OnMenuItem)
	EVT_MENU(VIEW_LIGHTING, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_OCT_TREE_ON_SURFACE, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_OCT_TREE_WIRE_FRAME, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_AGENT_VISION, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_AGENT_BRAIN, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_INVISIBLE_OBJECTS, SimulationWindow::OnMenuItem)
	
	// Debug
    EVT_MENU(DEBUG_SPAWN_AGENTS, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_DELETE_AGENT, SimulationWindow::OnMenuItem)

	// About
    EVT_MENU(wxID_ABOUT, SimulationWindow::OnMenuItem)
	
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

SimulationWindow::SimulationWindow() :
	wxFrame(NULL, wxID_ANY, wxT("New Simulation - SEAL")),
	m_simulationPanel(nullptr)
{
	// Setup the window UI.
	CreateUI();

	// Initialize a new simulation.
	m_simulationManager.Initialize();

	// Setup stuff.
	m_frameCounter = 0;
	double lastFrameTimeStamp = Time::GetTime();
	m_controlledAgentId = -1;
}


//-----------------------------------------------------------------------------
// UI
//-----------------------------------------------------------------------------

void SimulationWindow::CreateUI()
{
    SetIcon(wxICON(sample));
	SetTitle(wxT("New Simulation - SEAL"));
	
	// Create the render panel.
    m_simulationPanel = new SimulationRenderPanel(this, NULL);

    // Make a menubar
    wxMenuBar* menuBar = new wxMenuBar;
    SetMenuBar(menuBar);

	//-------------------------------------------------------------------------
	// FILE

    wxMenu* menuFile = new wxMenu;
    menuBar->Append(menuFile, wxT("&File"));
    menuFile->Append(wxID_NEW, "&New Simulation...\tCtrl+N");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_SAVE, "&Save Simulation Timeline...\tCtrl+S");
    menuFile->Append(wxID_OPEN, "&Open Simulation Timeline...\tCtrl+O");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE, "&Close\tCtrl+W");
	
	//-------------------------------------------------------------------------
	// SIMULATION

    wxMenu* menuSimulation = new wxMenu;
    menuBar->Append(menuSimulation, wxT("&Simulation"));
    menuSimulation->AppendCheckItem(PLAY_PAUSE_SIMULATION, "&Pause Simulation\tP")->Check(false);
    m_menuItemTickOnce = menuSimulation->Append(SIMULATION_TICK_ONCE, "&Tick once\t]");
    menuSimulation->AppendSeparator();
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_100, "&100% Speed")->Check(true);
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_200, "&200% Speed");
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_400, "&400% Speed");
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_MAX, "&Maximum Speed");
	
	m_menuItemTickOnce->Enable(false);
	
	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	// DEBUG

    wxMenu* menuDebug = new wxMenu;
    menuBar->Append(menuDebug, wxT("&Debug"));
    menuDebug->Append(DEBUG_SPAWN_AGENTS, "&Spawn Agents\tG");
    menuDebug->Append(DEBUG_DELETE_AGENT, "&Delete Selected Agent\tDelete");

	//-------------------------------------------------------------------------
	// HELP

    wxMenu* menuHelp = new wxMenu;
    menuBar->Append(menuHelp, wxT("&Help"));
    menuHelp->Append(wxID_ABOUT);

	// Make a status bar
    CreateStatusBar(4);
	
	// Set initial window size.
    SetClientSize(800, 600);
    Show();
}


//-----------------------------------------------------------------------------
// Window events
//-----------------------------------------------------------------------------

void SimulationWindow::OnNewWindow(wxCommandEvent& e)
{
    new SimulationWindow();
}

void SimulationWindow::OnClose(wxCommandEvent& e)
{
    Close(true); // true is to force the frame to close
}

void SimulationWindow::OnWindowClose(wxCloseEvent& e)
{
    Destroy(); // true is to force the frame to close
}


//-----------------------------------------------------------------------------
// Menu item events
//-----------------------------------------------------------------------------

void SimulationWindow::OnUpdateMenuItem(wxUpdateUIEvent& e)
{
	switch (e.GetId())
	{
	case SIMULATION_TICK_ONCE:
		m_menuItemTickOnce->Enable(m_simulationManager.IsSimulationPaused());
		break;
	}
}

void SimulationWindow::OnMenuItem(wxCommandEvent& e)
{
	switch (e.GetId())
	{
	//-------------------------------------------------------------------------
	// Simulation

	case PLAY_PAUSE_SIMULATION:
		m_simulationManager.SetSimulationPaused(e.IsChecked());
		//m_menuItemTickOnce->Enable(e.IsChecked());
		break;
	case SIMULATION_TICK_ONCE:
		m_simulationManager.TickSimulation();
		break;
	case SIMULATION_SPEED_100:
		m_simulationManager.SetTicksFerFrame(1);
		m_simulationManager.SetMaxTicksPerFrame(false);
		break;
	case SIMULATION_SPEED_200:
		m_simulationManager.SetTicksFerFrame(2);
		m_simulationManager.SetMaxTicksPerFrame(false);
		break;
	case SIMULATION_SPEED_400:
		m_simulationManager.SetTicksFerFrame(4);
		m_simulationManager.SetMaxTicksPerFrame(false);
		break;
	case SIMULATION_SPEED_MAX:
		m_simulationManager.SetMaxTicksPerFrame(true);
		break;
		
	//-------------------------------------------------------------------------
	// View

	case TOGGLE_CAMERA_TRACKING:
		m_simulationManager.ToggleCameraTracking();
		break;
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

	//-------------------------------------------------------------------------
	// Debug

	case DEBUG_SPAWN_AGENTS:
		for (int i = 0; i < 10; i++)
		{
			m_simulationManager.GetSimulation()->
				GetObjectManager()->SpawnObjectRandom<Agent>();
		}
		break;
	case DEBUG_DELETE_AGENT:
		if (m_simulationManager.GetSelectedAgent() != nullptr)
			m_simulationManager.GetSelectedAgent()->Destroy();
		break;

	//-------------------------------------------------------------------------
	// Help

	case wxID_ABOUT:
	{
		wxMessageBox("SEAL\nSimulation of Evolutionary Artificial Life.\n\nBy David Jordan & Ben Russel (2017)",
			"About SEAL", wxICON_INFORMATION);
		break;
	}
	}
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


//-----------------------------------------------------------------------------
// Updates
//-----------------------------------------------------------------------------

void SimulationWindow::OnIdle(wxIdleEvent& e)
{
	double startTime = Time::GetTime(); // time the update.
	
	// Update debug agent move keyboard controls.
	UpdateDebugAgentControls();

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
	
	double endTime = Time::GetTime();
	m_updateTimeMs = (endTime - startTime) * 1000.0;

	UpdateStatusBar();
	
	// Tell the simulation panel to render.
	m_simulationPanel->Refresh(false);
}

void SimulationWindow::UpdateDebugAgentControls()
{
	// Check if the use has selected a new agent.
	Agent* agent = m_simulationManager.GetSelectedAgent();
	if (m_controlledAgentId >= 0 && 
		(agent == nullptr || agent->GetId() != m_controlledAgentId))
	{
		Agent* controlledAgent = (Agent*) m_simulationManager
			.GetSimulation()->GetObjectManager()
			->GetObjectById(m_controlledAgentId);
		if (controlledAgent != nullptr)
			controlledAgent->SetManualOverride(false);
	}
	
	// Sync the controlled agent with the selected agent.
	if (agent != nullptr)
		m_controlledAgentId = agent->GetId();
	else
		m_controlledAgentId = -1;

	// Update movement controls for the currently selected agent.
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

}

void SimulationWindow::UpdateStatusBar()
{
	int statusIndex = 0;

	// 1. Number of simulation objects
	int numObjects = m_simulationManager.GetSimulation()->GetObjectManager()->GetNumObjects();
	std::stringstream ss;
	ss << numObjects << " objects";
	SetStatusText(ss.str(), statusIndex++);
	
	// 2. FPS.
	ss.str("");
	ss << (int) (m_fps + 0.5f) << " FPS";
	SetStatusText(ss.str(), statusIndex++);
	
	// 3. Update time in milliseconds
	ss.str("");
	ss.setf(std::ios::fixed, std::ios::floatfield);
	ss.precision(2);
	ss << "Update time: " << m_updateTimeMs << " ms";
	SetStatusText(ss.str(), statusIndex++);
	
	// 4. Render time in milliseconds
	double renderTimeMs = m_simulationManager.GetSimulationRenderer()->GetAverageRenderTime() * 1000.0;
	ss.str("");
	ss.setf(std::ios::fixed, std::ios::floatfield);
	ss.precision(2);
	ss << "Render time: " << renderTimeMs << " ms";
	SetStatusText(ss.str(), statusIndex++);
}
