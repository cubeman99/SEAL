#ifndef _GRAPH_CANVAS_H_
#define _GRAPH_CANVAS_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <math/Quaternion.h>
#include <math/Ray.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>

class SimulationWindow;
class SimulationManager;


//-----------------------------------------------------------------------------
// GraphCanvas
//-----------------------------------------------------------------------------
class GraphCanvas : public wxGLCanvas
{
public:
    GraphCanvas(wxWindow* parent, SimulationWindow* simulationWindow);
	~GraphCanvas();

	// Getters
	SimulationManager* GetSimulationManager();
	Simulation* GetSimulation();

	inline unsigned int GetGraphIndex() const { return m_graphIndex; }
	inline void SetGraphIndex(unsigned int graphIndex) { m_graphIndex = graphIndex; }


private:
	wxDECLARE_EVENT_TABLE();

	// UI Events
    void OnPaint(wxPaintEvent& e);


private:
	SimulationWindow* m_simulationWindow;
	unsigned int m_graphIndex;
};


#endif // _GRAPH_CANVAS_H_