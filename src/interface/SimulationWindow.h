#ifndef _SIMULATION_WINDOW_H_
#define _SIMULATION_WINDOW_H_

#include <wx/wx.h>
#include <simulation/Simulation.h>
#include <interface/SimulationRenderPanel.h>


// Define a new frame type
class SimulationWindow : public wxFrame
{
public:
    SimulationWindow();

private:
    void OnClose(wxCommandEvent& event);
    void OnClose2(wxCloseEvent& event);
    void OnNewWindow(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
	
	Simulation m_simulation;
	SimulationRenderPanel* m_simulationPanel;
};

enum
{
    NEW_STEREO_WINDOW = wxID_HIGHEST + 1,
	PAUSE_SIMULATION,
};


#endif // _SIMULATION_WINDOW_H_