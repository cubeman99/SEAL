#ifndef _SIMULATION_RENDER_PANEL_H_
#define _SIMULATION_RENDER_PANEL_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <simulation/World.h>
#include <simulation/Simulation.h>
#include <math/Quaternion.h>
#include <math/Ray.h>
#include <graphics/Shader.h>
#include <graphics/Renderer.h>


class SimulationWindow;
class SimulationManager;

// This is the panel where the simulation is drawn and can be interacted with.
class SimulationRenderPanel : public wxGLCanvas
{
public:
    SimulationRenderPanel(wxWindow* parent, int* attribList = NULL);
	~SimulationRenderPanel();

	SimulationManager* GetSimulationManager();
	Simulation* GetSimulation();


private:
	wxDECLARE_EVENT_TABLE();

    void OnKeyDown(wxKeyEvent& e);
	void OnMouseDown(wxMouseEvent& e);
	void OnMouseMotion(wxMouseEvent& e);
	void OnMouseWheel(wxMouseEvent& e);
	void OnSize(wxSizeEvent& e);
    void OnPaint(wxPaintEvent& e);

	SimulationWindow* m_simulationWindow;
	
	Renderer m_renderer;

	// Resources.
	Shader* m_shader;
	Shader* m_shaderUnlitVertexColored;
	Shader* m_shaderUnlitColored;
	Mesh* m_agentMesh;
	Mesh* m_meshSelectionCircle;
	Mesh* m_meshAxisLines;
	Material* m_worldMaterial;
	Material* m_agentMaterial;
	Material* m_materialSelectionCircle;
	Material* m_materialAxisLines;
};


#endif // _SIMULATION_RENDER_PANEL_H_