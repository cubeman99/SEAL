#include "SimulationWindow.h"
#include "MainApplication.h"
#include "SimulationRenderPanel.h"
#include <sstream>
#include <utilities/Timing.h>
#include "GraphCanvas.h"
#include <utilities/Logging.h>
#include <interface/dialogs/NewSimulationDialog.h>


//-----------------------------------------------------------------------------
// Event enumeration
//-----------------------------------------------------------------------------

enum
{
    CUSTOM_EVENT_IDS_BEGIN = wxID_HIGHEST + 1,

	//-------------------------------------------------------------------------


	CHOOSE_GRAPH,
	CHOOSE_HEAT_MAP_DATA,
	CHOOSE_HEAT_MAP_SPECIES,
	UPDATE_TIMER,
	
	//-------------------------------------------------------------------------
	MENU_ITEMS_BEGIN,

		// File
		FILE_NEW_SIMULATION,
		FILE_OPEN_SIMULATION,
		FILE_SAVE_SIMULATION,
		FILE_SAVE_SIMULATION_AS,
		FILE_CLOSE,

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
		SHOW_SKY_BOX,
		SHOW_PARTICLES,
		SET_LIGHT_DIRECTION,

		// Debug
		DEBUG_DEBUG_MODE,
		DEBUG_SPAWN_CARNIVORES,
		DEBUG_SPAWN_HERBIVORES,
		DEBUG_DELETE_AGENT,

		// Help
		HELP_ABOUT,
	
	MENU_ITEMS_END,
	//-------------------------------------------------------------------------
};


//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)

    EVT_IDLE(SimulationWindow::OnIdle)
    EVT_CLOSE(SimulationWindow::OnWindowClose)

	EVT_COMBOBOX(CHOOSE_GRAPH, SimulationWindow::OnChooseGraph)
	EVT_COMBOBOX(CHOOSE_HEAT_MAP_DATA, SimulationWindow::OnChooseHeatMapData)
	EVT_COMBOBOX(CHOOSE_HEAT_MAP_SPECIES, SimulationWindow::OnChooseHeatMapSpecies)

	EVT_UPDATE_UI_RANGE(MENU_ITEMS_BEGIN, MENU_ITEMS_END, SimulationWindow::OnUpdateMenuItem)
	//EVT_UPDATE_UI(SIMULATION_TICK_ONCE, SimulationWindow::OnUpdateMenuItem)
	//EVT_UPDATE_UI(TOGGLE_CAMERA_TRACKING, SimulationWindow::OnUpdateMenuItem)
	//EVT_UPDATE_UI(DEBUG_DELETE_AGENT, SimulationWindow::OnUpdateMenuItem)

	// File
	EVT_MENU(FILE_NEW_SIMULATION, SimulationWindow::OnNewSimulation)
    EVT_MENU(FILE_OPEN_SIMULATION, SimulationWindow::OnOpenSimulation)
    EVT_MENU(FILE_SAVE_SIMULATION, SimulationWindow::OnSaveSimulation)
    EVT_MENU(FILE_SAVE_SIMULATION_AS, SimulationWindow::OnSaveSimulation)
    EVT_MENU(FILE_CLOSE, SimulationWindow::OnClose)

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
	EVT_MENU(SHOW_SKY_BOX, SimulationWindow::OnMenuItem)
	EVT_MENU(SHOW_PARTICLES, SimulationWindow::OnMenuItem)
	EVT_MENU(SET_LIGHT_DIRECTION, SimulationWindow::OnMenuItem)
	
	// Debug
    EVT_MENU(DEBUG_DEBUG_MODE, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_SPAWN_CARNIVORES, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_SPAWN_HERBIVORES, SimulationWindow::OnMenuItem)
    EVT_MENU(DEBUG_DELETE_AGENT, SimulationWindow::OnMenuItem)

	// About
    EVT_MENU(HELP_ABOUT, SimulationWindow::OnMenuItem)
	
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
	// Heat Maps tab.

	m_pageHeatMaps = new wxWindow(m_tabControl, -1, wxDefaultPosition, wxDefaultSize, 0L);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		m_heatMapDataComboBox = new wxComboBox(m_pageHeatMaps, CHOOSE_HEAT_MAP_DATA,
			wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);

		int b = 2;

		wxBoxSizer* sizerData = new wxBoxSizer(wxHORIZONTAL);
		sizerData->Add(new wxStaticText(m_pageHeatMaps, -1, "Heat map data:"), 0, wxALL, b);
		sizerData->Add(m_heatMapDataComboBox, 1, wxEXPAND | wxALL, b);
		sizer->Add(sizerData, 0, wxEXPAND | wxALL, 0);

		m_heatMapSpeciesComboBox = new wxComboBox(m_pageHeatMaps, CHOOSE_HEAT_MAP_SPECIES,
			wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);

		wxBoxSizer* sizerSpeciesFilter = new wxBoxSizer(wxHORIZONTAL);
		sizerSpeciesFilter->Add(new wxStaticText(m_pageHeatMaps, -1, "Species filter:"), 0, wxALL, b);
		sizerSpeciesFilter->Add(m_heatMapSpeciesComboBox, 1, wxEXPAND | wxALL, b);
		sizer->Add(sizerSpeciesFilter, 0, wxEXPAND | wxALL, 0);

		m_pageHeatMaps->SetSizerAndFit(sizer);
	}
	m_tabControl->AddPage(m_pageHeatMaps, "Heat Maps");

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
    SetClientSize(1000, 640);
    Show();
}

void SimulationWindow::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();
    SetMenuBar(menuBar);
	
	//-------------------------------------------------------------------------
	// FILE

    wxMenu* menuFile = new wxMenu;
    menuBar->Append(menuFile, wxT("&File"));
    menuFile->Append(FILE_NEW_SIMULATION, "&New Simulation...\tCtrl+N");
    menuFile->AppendSeparator();
    menuFile->Append(FILE_SAVE_SIMULATION, "&Save Simulation...\tCtrl+S");
    //menuFile->Append(FILE_SAVE_SIMULATION_AS, "&Save Simulation...\tCtrl+Shift+S");
    menuFile->Append(FILE_OPEN_SIMULATION, "&Open Simulation...\tCtrl+O");
    menuFile->AppendSeparator();
    menuFile->Append(FILE_CLOSE, "&Close");
	
	//-------------------------------------------------------------------------
	// SIMULATION

    wxMenu* menuSimulation = new wxMenu;
    menuBar->Append(menuSimulation, wxT("&Simulation"));
    m_menuItemPauseSimulation = menuSimulation->AppendCheckItem(PLAY_PAUSE_SIMULATION, "&Pause Simulation\tP");
    m_menuItemTickOnce = menuSimulation->Append(SIMULATION_TICK_ONCE, "&Tick once\t]");
    menuSimulation->AppendSeparator();
    m_menuItemSimulationSpeed100 = menuSimulation->AppendRadioItem(SIMULATION_SPEED_100, "&100% Speed\t1");
    m_menuItemSimulationSpeed200 = menuSimulation->AppendRadioItem(SIMULATION_SPEED_200, "&200% Speed\t2");
    m_menuItemSimulationSpeed400 = menuSimulation->AppendRadioItem(SIMULATION_SPEED_400, "&400% Speed\t3");
    m_menuItemSimulationSpeedMax = menuSimulation->AppendRadioItem(SIMULATION_SPEED_MAX, "&Maximum Speed\t0");
	
	//-------------------------------------------------------------------------
	// VIEW

    wxMenu* menuView = new wxMenu;
    menuBar->Append(menuView, wxT("&View"));
    m_menuItemCameraTracking = menuView->AppendCheckItem(TOGGLE_CAMERA_TRACKING, "Camera &Tracking\tT");
	menuView->AppendSeparator();
    m_menuItemWireFrameMode = menuView->AppendCheckItem(VIEW_WIREFRAME_MODE, "&Wireframe mode\tW");
    m_menuItemViewLighting = menuView->AppendCheckItem(VIEW_LIGHTING, "&Lighting\tL");
	menuView->AppendSeparator();
    m_menuItemShowOctTreeOnSurface = menuView->AppendCheckItem(SHOW_OCT_TREE_ON_SURFACE, "Show OctTree on &Surface\tO");
    m_menuItemShowOctTreeWireFrame = menuView->AppendCheckItem(SHOW_OCT_TREE_WIRE_FRAME, "Show Oct&Tree Wireframe\tShift+O");
    m_menuItemShowAgentVision = menuView->AppendCheckItem(SHOW_AGENT_VISION, "Show Agent &Vision\tA");
    m_menuItemShowAgentBrain = menuView->AppendCheckItem(SHOW_AGENT_BRAIN, "Show Agent &Brain\tB");
    m_menuItemShowInvisibleObjects = menuView->AppendCheckItem(SHOW_INVISIBLE_OBJECTS, "Show &Invisible Objects\tI");
    m_menuItemShowSkyBox = menuView->AppendCheckItem(SHOW_SKY_BOX, "Show S&kybox");
    m_menuItemShowParticles = menuView->AppendCheckItem(SHOW_PARTICLES, "Show &Particles");
    menuView->AppendCheckItem(SET_LIGHT_DIRECTION, "Set Light &Direction\tCtrl+L");

	//-------------------------------------------------------------------------
	// DEBUG

    wxMenu* menuDebug = new wxMenu;
    menuBar->Append(menuDebug, wxT("&Debug"));
    m_menuItemDebugMode = menuDebug->AppendCheckItem(DEBUG_DEBUG_MODE, "Debug &Mode");
	menuDebug->AppendSeparator();
    m_menuItemSpawnCarnivores = menuDebug->Append(DEBUG_SPAWN_CARNIVORES, "&Spawn Carnivore Agents\tG");
    m_menuItemSpawnHerbivores = menuDebug->Append(DEBUG_SPAWN_HERBIVORES, "&Spawn Herbivore Agents\tH");
    m_menuItemDeleteAgent = menuDebug->Append(DEBUG_DELETE_AGENT, "&Delete Agent\tDelete");

	//-------------------------------------------------------------------------
	// HELP

    wxMenu* menuHelp = new wxMenu;
    menuBar->Append(menuHelp, wxT("&Help"));
    menuHelp->Append(HELP_ABOUT, "About");
}

void SimulationWindow::OnNewSimulation()
{
	GraphManager* graphManager = m_simulationManager.GetGraphManager();
	HeatMapManager* heatMapManager = m_simulationManager.GetHeatMapManager();
	
	// Graphs
	m_graphComboBox->Clear();
	for (unsigned int i = 0; i < graphManager->GetNumGraphs(); ++i)
		m_graphComboBox->Append(graphManager->GetGraph(i)->GetTitle());
	m_graphComboBox->SetSelection(0);
	m_graphCanvas->SetGraphIndex(0);

	// Heat map data
	m_heatMapDataComboBox->Clear();
	m_heatMapDataComboBox->Append("None");
	for (unsigned int i = 0; i < heatMapManager->GetNumHeatMap(); ++i)
		m_heatMapDataComboBox->Append(heatMapManager->GetHeatMap(i)->GetTitle());
	m_heatMapDataComboBox->SetSelection(0);
	m_simulationManager.SetActiveHeatMapIndex(-1);

	// Heat map species filter
	m_heatMapSpeciesComboBox->Clear();
	m_heatMapSpeciesComboBox->Append("Herbivores");
	m_heatMapSpeciesComboBox->Append("Carnivores");
	m_heatMapSpeciesComboBox->Append("Both");
	m_heatMapSpeciesComboBox->SetSelection(2);
	m_simulationManager.SetHeatMapSpeciesFilter(SPECIES_FILTER_BOTH);
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
	//-------------------------------------------------------------------------
	// Simulation

	case PLAY_PAUSE_SIMULATION:
		m_menuItemPauseSimulation->Check(m_simulationManager.IsSimulationPaused());
		break;
	case SIMULATION_TICK_ONCE:
		m_menuItemTickOnce->Enable(m_simulationManager.IsSimulationPaused());
		break;
	case SIMULATION_SPEED_100:
		m_menuItemSimulationSpeed100->Check(m_simulationManager.GetTicksFerFrame() == 1);
		break;
	case SIMULATION_SPEED_200:
		m_menuItemSimulationSpeed200->Check(m_simulationManager.GetTicksFerFrame() == 2);
		break;
	case SIMULATION_SPEED_400:
		m_menuItemSimulationSpeed400->Check(m_simulationManager.GetTicksFerFrame() == 4);
		break;
	case SIMULATION_SPEED_MAX:
		m_menuItemSimulationSpeedMax->Check(m_simulationManager.GetMaxTicksPerFrame());
		break;
		
	//-------------------------------------------------------------------------
	// View
		
	case TOGGLE_CAMERA_TRACKING:
		m_menuItemCameraTracking->Check(
			m_simulationManager.GetCameraSystem()->IsTrackingObject());
		break;
	case VIEW_WIREFRAME_MODE:
		m_menuItemWireFrameMode->Check(
			m_simulationManager.IsViewWireFrameMode());
		break;
	case VIEW_LIGHTING:
		m_menuItemViewLighting->Check(
			m_simulationManager.IsLightingEnabled());
		break;
	case SHOW_OCT_TREE_ON_SURFACE:
		m_menuItemShowOctTreeOnSurface->Check(
			m_simulationManager.GetShowOctTree());
		break;
	case SHOW_OCT_TREE_WIRE_FRAME:
		m_menuItemShowOctTreeWireFrame->Check(
			m_simulationManager.GetShowOctTreeWireFrame());
		break;
	case SHOW_AGENT_VISION:
		m_menuItemShowAgentVision->Check(
			m_simulationManager.GetShowAgentVision());
		break;
	case SHOW_AGENT_BRAIN:
		m_menuItemShowAgentBrain->Check(
			m_simulationManager.GetShowAgentBrain());
		break;
	case SHOW_INVISIBLE_OBJECTS:
		m_menuItemShowInvisibleObjects->Check(
			m_simulationManager.GetShowInvisibleObjects());
		break;
	case SHOW_SKY_BOX:
		m_menuItemShowSkyBox->Check(
			m_simulationManager.GetShowSkyBox());
		break;
	case SHOW_PARTICLES:
		m_menuItemShowParticles->Check(
			m_simulationManager.GetShowParticles());
		break;
		
	//-------------------------------------------------------------------------
	// Debug

	case DEBUG_DEBUG_MODE:
		m_menuItemDebugMode->Check(m_simulationManager.IsDebugMode());
		break;
	case DEBUG_SPAWN_CARNIVORES:
		m_menuItemSpawnCarnivores->Enable(m_simulationManager.IsDebugMode());
		break;
	case DEBUG_SPAWN_HERBIVORES:
		m_menuItemSpawnHerbivores->Enable(m_simulationManager.IsDebugMode());
		break;
	case DEBUG_DELETE_AGENT:
		m_menuItemDeleteAgent->Enable(
			m_simulationManager.IsDebugMode() &&
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
	case SHOW_SKY_BOX:
		m_simulationManager.SetShowSkyBox(e.IsChecked());
		break;
	case SHOW_PARTICLES:
		m_simulationManager.SetShowParticles(e.IsChecked());
		break;
	case SET_LIGHT_DIRECTION:
		m_simulationManager.GetSimulationRenderer()->GetRenderer()->SetLightDirection(
			m_simulationManager.GetActiveCamera()->GetOrientation().GetForward());
		break;

	//-------------------------------------------------------------------------
	// Debug
		
	case DEBUG_DEBUG_MODE:
	{
		if (e.IsChecked())
		{
			// Ask the user if they want to activate debug mode
			wxMessageDialog dialog(this,
				"Are you sure you want to activate Debug Mode?\n"
				"Using debug controls will interfere with a simulation's evolutionary track, "
				"making it susceptible to a more difficult or less valid interpretion.",
				"Activate Debug Mode",
				wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
			if (dialog.ShowModal() == wxID_YES)
			{
				m_simulationManager.SetDebugMode(true);
			}
		}
		else
		{
			m_simulationManager.SetDebugMode(false);
		}
		break;
	}
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

	case HELP_ABOUT:
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

void SimulationWindow::OnChooseHeatMapData(wxCommandEvent& e)
{
	m_simulationManager.SetActiveHeatMapIndex(e.GetSelection() - 1);
}

void SimulationWindow::OnChooseHeatMapSpecies(wxCommandEvent& e)
{
	m_simulationManager.SetHeatMapSpeciesFilter(
		(SpeciesFilter) e.GetSelection());
}

void SimulationWindow::OnClose(wxCommandEvent& e)
{
    Close(true); // true is to force the frame to close
}

void SimulationWindow::OnNewSimulation(wxCommandEvent& e)
{
	NewSimulationDialog dialog(this, wxID_ANY, "New Simulation");
	
	if (dialog.ShowModal() == wxID_OK)
	{
		// Begin a new simulation with loaded config.
		m_simulationManager.BeginNewSimulation(dialog.GetSimulationConfig());
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
	
	// Relinquish control over the agent if were not in debug mode.
	if (!m_simulationManager.IsDebugMode())
	{
		if (agent != nullptr)
			agent->SetManualOverride(false);
		return;
	}

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
