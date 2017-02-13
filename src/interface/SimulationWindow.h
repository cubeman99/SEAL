#ifndef _SIMULATION_WINDOW_H_
#define _SIMULATION_WINDOW_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <application/SimulationManager.h>
#include <simulation/Simulation.h>
#include "SimulationRenderPanel.h"


//-----------------------------------------------------------------------------
// SimulationWindow - The main program window (there can be more than one).
//-----------------------------------------------------------------------------
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
    void OnMenuItem(wxCommandEvent& e);
    void OnUpdateTimer(wxTimerEvent& e);
    void OnWindowClose(wxCloseEvent& e);

	
	SimulationManager m_simulationManager;

	SimulationRenderPanel* m_simulationPanel;

	Agent*	m_controlledAgent;

    wxTimer	m_updateTimer;
	int		m_frameCounter;
	float	m_fps;
	double	m_lastFrameTimeStamp;
};


#endif // _SIMULATION_WINDOW_H_