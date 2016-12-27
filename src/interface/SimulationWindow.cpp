#include "SimulationWindow.h"
#include "MainApplication.h"
#include "SimulationRenderPanel.h"


wxBEGIN_EVENT_TABLE(SimulationWindow, wxFrame)
    EVT_MENU(wxID_NEW, SimulationWindow::OnNewWindow)
    EVT_MENU(wxID_CLOSE, SimulationWindow::OnClose)
	
    //EVT_MENU(wxID_ABOUT, SimulationWindow::OnAbout)
wxEND_EVENT_TABLE()

SimulationWindow::SimulationWindow() :
	   wxFrame(NULL, wxID_ANY, wxT("wxWidgets OpenGL Cube Sample"))
{
    new SimulationRenderPanel(this, NULL);

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenuBar* menuBar = new wxMenuBar;
    SetMenuBar(menuBar);

	// FILE
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_NEW);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE);
    menuBar->Append(menuFile, wxT("&File"));
	
	// SIMULATION
    wxMenu* menuSimulation = new wxMenu;
    menuSimulation->Append(PAUSE_SIMULATION, "&Pause Simulation");
    menuBar->Append(menuSimulation, wxT("&Simulation"));

	// VIEW
    wxMenu* menuView = new wxMenu;
    menuView->Append(wxID_ABOUT);
    menuBar->Append(menuView, wxT("&View"));

	// HELP
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    menuBar->Append(menuHelp, wxT("&Help"));

	// Make a status bar
    CreateStatusBar();
	SetStatusText("Hello");

    SetClientSize(640, 480);
    Show();
}

void SimulationWindow::OnClose(wxCommandEvent& e)
{
    Close(true); // true is to force the frame to close
}

void SimulationWindow::OnNewWindow(wxCommandEvent& e)
{
    new SimulationWindow();
}

