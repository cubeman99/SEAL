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
    void OnNewWindow(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);
    void OnPlayPauseSimulation(wxCommandEvent& event);
    void OnToggleCameraTracking(wxCommandEvent& event);
    void OnSpawnAgents(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
	
	Simulation m_simulation;
	SimulationRenderPanel* m_simulationPanel;
};


#endif // _SIMULATION_WINDOW_H_