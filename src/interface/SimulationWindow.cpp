#include "SimulationWindow.h"
#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include <sstream>
#include <utilities/Timing.h>
#include "GraphCanvas.h"


//-----------------------------------------------------------------------------
// Event enumeration
//-----------------------------------------------------------------------------

enum
{
    NEW_STEREO_WINDOW = wxID_HIGHEST + 1,

	CHOOSE_GRAPH,

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
	DEBUG_SPAWN_CARNIVORES,
	DEBUG_SPAWN_HERBIVORES,
	DEBUG_DELETE_AGENT,

	// Help
	// (using default wxIDs)

	UPDATE_TIMER,
};


//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)

    EVT_IDLE(SimulationWindow::OnIdle)
    EVT_CLOSE(SimulationWindow::OnWindowClose)

	EVT_COMBOBOX(CHOOSE_GRAPH, SimulationWindow::OnChooseGraph)

	EVT_UPDATE_UI(SIMULATION_TICK_ONCE, SimulationWindow::OnUpdateMenuItem)
	EVT_UPDATE_UI(TOGGLE_CAMERA_TRACKING, SimulationWindow::OnUpdateMenuItem)
	EVT_UPDATE_UI(DEBUG_DELETE_AGENT, SimulationWindow::OnUpdateMenuItem)

	// File
	EVT_MENU(wxID_NEW, SimulationWindow::OnNewSimulation)
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
    EVT_MENU(DEBUG_SPAWN_CARNIVORES, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_SPAWN_HERBIVORES, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_DELETE_AGENT, SimulationWindow::OnMenuItem)

	// About
    EVT_MENU(wxID_ABOUT, SimulationWindow::OnMenuItem)
	
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

SimulationWindow::SimulationWindow() :
	wxFrame(NULL, wxID_ANY, wxT("New Simulation - SEAL")),
	m_simulationPanel(nullptr),
	m_splitter(nullptr),
	m_tabControl(nullptr),
	m_pageInfo(nullptr),
	m_infoPanelCanvas(nullptr),
	m_pageGraphs(nullptr),
	m_graphComboBox(nullptr),
	m_graphCanvas(nullptr),
	m_pageLog(nullptr),
	m_logBox(nullptr)
{
	// Setup the window UI.
	CreateUI();

	// Initialize a new simulation.
	m_simulationManager.Initialize();
	OnNewSimulation();

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
	//-------------------------------------------------------------------------
	// Create vertically-split layout.

	m_splitter = new wxSplitterWindow(this, -1, wxDefaultPosition,
		wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
	
    m_simulationPanel = new SimulationRenderPanel(m_splitter, this);

	m_tabControl = new wxNotebook(m_splitter, -1, wxDefaultPosition, wxDefaultSize);

	m_splitter->SplitVertically(m_simulationPanel, m_tabControl, -100);
	m_splitter->SetMinimumPaneSize(100);
	m_splitter->SetSashPosition(-300);
	m_splitter->SetSashGravity(1.0);
	
	//-------------------------------------------------------------------------
	// Info tab.

	m_pageInfo = new wxWindow(m_tabControl, -1, wxDefaultPosition, wxDefaultSize, 0L);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		
		m_infoPanelCanvas = new InfoPanelCanvas(m_pageInfo, this);
		sizer->Add(m_infoPanelCanvas, 1, wxEXPAND | wxALL, 0);

		m_pageInfo->SetSizerAndFit(sizer);
	}
	m_tabControl->AddPage(m_pageInfo, "Info", true);
	
	//-------------------------------------------------------------------------
	// Graphs tab.

	m_pageGraphs = new wxWindow(m_tabControl, -1, wxDefaultPosition, wxDefaultSize, 0L);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		
		m_graphComboBox = new wxComboBox(m_pageGraphs, CHOOSE_GRAPH,
			wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
		m_graphComboBox->SetSelection(0);
		sizer->Add(m_graphComboBox, 0, wxEXPAND | wxALL, 0);
	
		m_graphCanvas = new GraphCanvas(m_pageGraphs, this);
		m_graphCanvas->SetSize(wxSize(100, 200));
		sizer->Add(m_graphCanvas, 1, wxEXPAND | wxALL, 0);

		m_pageGraphs->SetSizerAndFit(sizer);
	}
	m_tabControl->AddPage(m_pageGraphs, "Graphs");

	//-------------------------------------------------------------------------
	// Log tab.

	m_pageLog = new wxWindow(m_tabControl, -1, wxDefaultPosition, wxDefaultSize, 0L);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		
		m_logBox = new wxTextCtrl(m_pageLog, -1, wxEmptyString,
			wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE  |wxTE_READONLY);
		sizer->Add(m_logBox, 1, wxEXPAND | wxALL, 0);

		wxLog::SetActiveTarget(new wxLogTextCtrl(m_logBox));

		m_pageLog->SetSizerAndFit(sizer);
	}
	m_tabControl->AddPage(m_pageLog, "Log");

	//-------------------------------------------------------------------------

    CreateMenuBar();
    CreateStatusBar(4);

    SetIcon(wxICON(sample));
	SetTitle(wxT("New Simulation - SEAL"));
	
	// Set initial window size.
    SetClientSize(1000, 600);
    Show();

	// TODO: remove this test message.
	wxLogMessage("Test message using wxLogMessage()");
}

void SimulationWindow::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();
    SetMenuBar(menuBar);
	
	//-------------------------------------------------------------------------
	// FILE

    wxMenu* menuFile = new wxMenu;
    menuBar->Append(menuFile, wxT("&File"));
    menuFile->Append(wxID_NEW, "&New Simulation...\tCtrl+N");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_SAVE, "&Save Simulation...\tCtrl+S");
    menuFile->Append(wxID_OPEN, "&Open Simulation...\tCtrl+O");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE, "&Close\tCtrl+W");
	
	//-------------------------------------------------------------------------
	// SIMULATION

    wxMenu* menuSimulation = new wxMenu;
    menuBar->Append(menuSimulation, wxT("&Simulation"));
    menuSimulation->AppendCheckItem(PLAY_PAUSE_SIMULATION, "&Pause Simulation\tP")->Check(false);
    m_menuItemTickOnce = menuSimulation->Append(SIMULATION_TICK_ONCE, "&Tick once\t]");
    menuSimulation->AppendSeparator();
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_100, "&100% Speed\t1")->Check(true);
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_200, "&200% Speed\t2");
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_400, "&400% Speed\t3");
    menuSimulation->AppendRadioItem(SIMULATION_SPEED_MAX, "&Maximum Speed\t0");
	
	//-------------------------------------------------------------------------
	// VIEW

    wxMenu* menuView = new wxMenu;
    menuBar->Append(menuView, wxT("&View"));
    m_menuItemCameraTracking = menuView->AppendCheckItem(TOGGLE_CAMERA_TRACKING, "Camera &Tracking\tT");
	menuView->AppendSeparator();
    menuView->AppendCheckItem(VIEW_WIREFRAME_MODE, "&Wireframe mode\tW");
    menuView->AppendCheckItem(VIEW_LIGHTING, "&Lighting\tL")->Check(true);
	menuView->AppendSeparator();
    menuView->AppendCheckItem(SHOW_OCT_TREE_ON_SURFACE, "Show OctTree on &Surface\tO")->Check(false);
    menuView->AppendCheckItem(SHOW_OCT_TREE_WIRE_FRAME, "Show Oct&Tree Wireframe\tShift+O")->Check(false);
    menuView->AppendCheckItem(SHOW_AGENT_VISION, "Show Agent &Vision\tA")->Check(false);
    menuView->AppendCheckItem(SHOW_AGENT_BRAIN, "Show Agent &Brain\tB")->Check(false);
    menuView->AppendCheckItem(SHOW_INVISIBLE_OBJECTS, "Show &Invisible Objects\tI")->Check(false);

	//-------------------------------------------------------------------------
	// DEBUG

    wxMenu* menuDebug = new wxMenu;
    menuBar->Append(menuDebug, wxT("&Debug"));
    menuDebug->Append(DEBUG_SPAWN_CARNIVORES, "&Spawn Carnivore Agents\tG");
    menuDebug->Append(DEBUG_SPAWN_HERBIVORES, "&Spawn Herbivore Agents\tH");
    m_menuItemDeleteAgent = menuDebug->Append(DEBUG_DELETE_AGENT, "&Delete Agent\tDelete");

	//-------------------------------------------------------------------------
	// HELP

    wxMenu* menuHelp = new wxMenu;
    menuBar->Append(menuHelp, wxT("&Help"));
    menuHelp->Append(wxID_ABOUT);
}

void SimulationWindow::OnNewSimulation()
{
	GraphManager* graphManager = m_simulationManager.GetGraphManager();

	m_graphComboBox->Clear();
	for (unsigned int i = 0; i < graphManager->GetNumGraphs(); ++i)
		m_graphComboBox->Append(graphManager->GetGraph(i)->GetTitle());
	m_graphComboBox->SetSelection(0);
	m_graphCanvas->SetGraphIndex(0);
}


//-----------------------------------------------------------------------------
// Window events
//-----------------------------------------------------------------------------

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
	case TOGGLE_CAMERA_TRACKING:
		m_menuItemCameraTracking->Check(
			m_simulationManager.GetCameraSystem()->IsTrackingObject());
	case DEBUG_DELETE_AGENT:
		m_menuItemDeleteAgent->Enable(
			m_simulationManager.GetSelectedAgent() != nullptr);
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
		m_simulationManager.SetCameraTracking(e.IsChecked());
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
		
	case DEBUG_SPAWN_CARNIVORES:
		for (int i = 0; i < 10; i++)
		{
			m_simulationManager.GetSimulation()->
				GetObjectManager()->SpawnObjectRandom(new Agent(SPECIES_CARNIVORE), true);
		}
		break;
	case DEBUG_SPAWN_HERBIVORES:
		for (int i = 0; i < 10; i++)
		{
			m_simulationManager.GetSimulation()->
				GetObjectManager()->SpawnObjectRandom(new Agent(SPECIES_HERBIVORE), true);
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
		wxMessageBox("SEAL\nSimulation of Evolutionary Artificial Life.\n\nBy David Jordan & Ben Russell (2017)",
			"About SEAL", wxICON_INFORMATION);
		break;
	}
	}
}

void SimulationWindow::OnChooseGraph(wxCommandEvent& e)
{
	m_graphCanvas->SetGraphIndex(e.GetSelection());
}

void SimulationWindow::OnClose(wxCommandEvent& e)
{
    Close(true); // true is to force the frame to close
}

void SimulationWindow::OnNewSimulation(wxCommandEvent& e)
{
	// TODO: New simulation dialog
	//   - specify path to config file.
	//      -OR-
	//   - dialog box for editing config params within app
	
	// Ask the user if they're sure what they're doing.
	wxMessageDialog dialog(this,
		"Are you sure you want to begin a new simulation?",
		"New Simulation", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT);
	if (dialog.ShowModal() == wxID_YES)
	{
		// TEMP: begin a new simulation with default config values.
		SimulationConfig config;
		m_simulationManager.BeginNewSimulation(config);
		OnNewSimulation();
	}
}

void SimulationWindow::OnOpenSimulation(wxCommandEvent& e)
{
	wxFileDialog* openDialog = new wxFileDialog(this,
		"Open Simulation", wxEmptyString, wxEmptyString, 
		"Simulation files (*.bin)|*.bin",
		wxFD_OPEN, wxDefaultPosition);
 
	if (openDialog->ShowModal() == wxID_OK)
	{
		std::string path = (std::string) openDialog->GetPath();
		
		if (m_simulationManager.OpenSimulation(path))
		{
			// Update the window title to the opened simulation file.
			SetTitle(openDialog->GetFilename() << wxString(" - SEAL"));
			OnNewSimulation();
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
		"Save Simulation", wxEmptyString, wxEmptyString, 
		"Simulation files (*.bin)|*.bin",
		wxFD_SAVE, wxDefaultPosition);
 
	if (saveDialog->ShowModal() == wxID_OK)
	{
		std::string path = (std::string) saveDialog->GetPath();
		
		if (m_simulationManager.SaveSimulation(path))
		{
			// success!
			OnNewSimulation();
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
	m_graphCanvas->Refresh(false);
	m_infoPanelCanvas->Refresh(false);
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
