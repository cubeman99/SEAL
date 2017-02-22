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

	// Getters
	inline SimulationManager* GetSimulationManager() { return &m_simulationManager; }
	inline SimulationRenderPanel* GetRenderPanel() { return m_simulationPanel; }


private:
    wxDECLARE_EVENT_TABLE();
	
	// UI
	void CreateUI();
	
	// Window events
    void OnNewWindow(wxCommandEvent& e);
    void OnClose(wxCommandEvent& e);
    void OnWindowClose(wxCloseEvent& e);

	// Menu item events
	void OnUpdateMenuItem(wxUpdateUIEvent& e);
    void OnMenuItem(wxCommandEvent& e);
	void OnOpenSimulation(wxCommandEvent& e);
	void OnSaveSimulation(wxCommandEvent& e);

	// Updates
	void OnIdle(wxIdleEvent& e);
	void UpdateStatusBar();
	void UpdateDebugAgentControls();


private:
	SimulationManager m_simulationManager;
	SimulationRenderPanel* m_simulationPanel;

	int		m_controlledAgentId;
	int		m_frameCounter;
	float	m_fps;
	double	m_lastFrameTimeStamp;
	double	m_updateTimeMs;

	// Menu items
	wxMenuItem* m_menuItemTickOnce;
};


#endif // _SIMULATION_WINDOW_H_