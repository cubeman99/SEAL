#ifndef _SIMULATION_WINDOW_H_
#define _SIMULATION_WINDOW_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <application/SimulationManager.h>
#include <simulation/Simulation.h>
#include "SimulationRenderPanel.h"
#include "GraphCanvas.h"
#include "InfoPanelCanvas.h"
#include <wx/notebook.h>
#include <wx/splitter.h>


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

	void OnNewSimulation();


private:
    wxDECLARE_EVENT_TABLE();
	
	// UI
	void CreateUI();
	void CreateMenuBar();
	
	// Window events
    void OnWindowClose(wxCloseEvent& e);

	// Menu item events
	void OnUpdateMenuItem(wxUpdateUIEvent& e);
    void OnMenuItem(wxCommandEvent& e);
	void OnChooseGraph(wxCommandEvent& e);
	void OnChooseHeatMapData(wxCommandEvent& e);
	void OnChooseHeatMapSpecies(wxCommandEvent& e);
    void OnClose(wxCommandEvent& e);
    void OnNewSimulation(wxCommandEvent& e);
	void OnOpenSimulation(wxCommandEvent& e);
	void OnSaveSimulation(wxCommandEvent& e);

	// Updates
	void OnIdle(wxIdleEvent& e);
	void UpdateStatusBar();
	void UpdateDebugAgentControls();


private:
	SimulationManager m_simulationManager;

	int		m_controlledAgentId;
	int		m_frameCounter;
	float	m_fps;
	double	m_lastFrameTimeStamp;
	double	m_updateTimeMs;

	// Window hierarchy
	wxSplitterWindow*				m_splitter;
		SimulationRenderPanel*		m_simulationPanel;
		wxNotebook*					m_tabControl;
			wxWindow*				m_pageInfo;
				InfoPanelCanvas*	m_infoPanelCanvas;
			wxWindow*				m_pageGraphs;
				GraphCanvas*		m_graphCanvas;
				wxComboBox*			m_graphComboBox;
			wxWindow*				m_pageLog;
				wxTextCtrl*			m_logBox;
			wxWindow*				m_pageHeatMaps;
				wxComboBox*			m_heatMapDataComboBox;
				wxComboBox*			m_heatMapSpeciesComboBox;

	// Simulation
	wxMenuItem* m_menuItemPauseSimulation;
	wxMenuItem* m_menuItemTickOnce;
	wxMenuItem* m_menuItemSimulationSpeed100;
	wxMenuItem* m_menuItemSimulationSpeed200;
	wxMenuItem* m_menuItemSimulationSpeed400;
	wxMenuItem* m_menuItemSimulationSpeedMax;
	
	// View
	wxMenuItem* m_menuItemCameraTracking;
	wxMenuItem* m_menuItemWireFrameMode;
	wxMenuItem* m_menuItemViewLighting;
	wxMenuItem* m_menuItemShowOctTreeOnSurface;
	wxMenuItem* m_menuItemShowOctTreeWireFrame;
	wxMenuItem* m_menuItemShowAgentVision;
	wxMenuItem* m_menuItemShowAgentBrain;
	wxMenuItem* m_menuItemShowInvisibleObjects;

	// Debug
	wxMenuItem* m_menuItemDebugMode;
	wxMenuItem* m_menuItemSpawnCarnivores;
	wxMenuItem* m_menuItemSpawnHerbivores;
	wxMenuItem* m_menuItemDeleteAgent;
};


#endif // _SIMULATION_WINDOW_H_