#ifndef _SIMULATION_WINDOW_H_
#define _SIMULATION_WINDOW_H_

#include <wx/wx.h>
#include <simulation/Simulation.h>


// Define a new frame type
class SimulationWindow : public wxFrame
{
public:
    SimulationWindow();

private:
    void OnClose(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
	
	Simulation m_simulation;
};

enum
{
    NEW_STEREO_WINDOW = wxID_HIGHEST + 1,
	PAUSE_SIMULATION,
};


#endif // _SIMULATION_WINDOW_H_