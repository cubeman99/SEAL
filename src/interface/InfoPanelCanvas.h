#ifndef _INFO_PANEL_CANVAS_H_
#define _INFO_PANEL_CANVAS_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <math/Quaternion.h>
#include <math/Ray.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include <application/InfoPanel.h>

class SimulationWindow;
class SimulationManager;


//-----------------------------------------------------------------------------
// InfoPanelCanvas
//-----------------------------------------------------------------------------
class InfoPanelCanvas : public wxGLCanvas
{
public:
    InfoPanelCanvas(wxWindow* parent, SimulationWindow* simulationWindow);
	~InfoPanelCanvas();

	// Getters
	SimulationManager* GetSimulationManager();
	Simulation* GetSimulation();


private:
	wxDECLARE_EVENT_TABLE();

	// UI Events
    void OnPaint(wxPaintEvent& e);


private:
	SimulationWindow* m_simulationWindow;

	InfoPanel m_agentInfoPanel;
	InfoPanel m_simInfoPanel;
};


#endif // _INFO_PANEL_CANVAS_H_