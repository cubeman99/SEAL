#ifndef _SIMULATION_WINDOW_H_
#define _SIMULATION_WINDOW_H_

#include <wx/wx.h>
#include <simulation/Simulation.h>
#include <interface/SimulationRenderPanel.h>
#include <interface/SimulationManager.h>


// Define a new frame type
class SimulationWindow : public wxFrame
{
public:
    SimulationWindow();

	inline SimulationManager* GetSimulationManager() { return &m_simulationManager; }

private:
    wxDECLARE_EVENT_TABLE();

    void OnClose(wxCommandEvent& e);
    void OnNewWindow(wxCommandEvent& e);
    void OnPlayPauseSimulation(wxCommandEvent& e);
    void OnToggleCameraTracking(wxCommandEvent& e);
    void OnSpawnAgents(wxCommandEvent& e);

    void OnUpdateTimer(wxTimerEvent& e);

    void OnWindowClose(wxCloseEvent& e);
	
	SimulationManager m_simulationManager;

    wxTimer m_updateTimer;
	
	SimulationRenderPanel* m_simulationPanel;
};


#endif // _SIMULATION_WINDOW_H_