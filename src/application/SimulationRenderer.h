#ifndef _SIMULATION_RENDERER_H_
#define _SIMULATION_RENDERER_H_	

#include <graphics/OpenGLIncludes.h>
#include <graphics/Graphics.h>
#include <math/MathLib.h>
#include "ResourceManager.h"
#include "OctTreeRenderer.h"
#include "Graph.h"
#include "InfoPanel.h"


class SimulationManager;
class Agent;


class SimulationRenderer
{
public:
	SimulationRenderer();
	~SimulationRenderer();

	void Initialize(SimulationManager* simulationManager);

	void Render(const Vector2f& viewPortSize);

	void RenderAgentVisionArcs(Agent* agent);
	void RenderAgentVisionStrips(Agent* agent);
	void RenderBrain(Agent* agent);
	void RenderGraphs();
	void RenderInfoPanel();

	inline double GetAverageRenderTime() const { return m_renderTime; }

private:
	SimulationManager* m_simulationManager;
	ResourceManager	m_resourceManager;
	Renderer		m_renderer; // Used to render 3d meshes
	Graphics		m_graphics; // used to draw 2d shapes
	OctTreeRenderer	m_octTreeRenderer;

	double			m_renderTime; // measured time of render (in seconds)

	Vector2f		m_viewPortSize;


	GraphPanel		m_graphFitness;

	// Resources.
	Font* m_font;
	Mesh* m_agentMesh;
	Mesh* m_plantMesh;
	Mesh* m_worldMesh;
	Mesh* m_meshSelectionCircle;
	Mesh* m_meshAxisLines;
	Material* m_worldMaterial;
	Material* m_agentMaterial;
	Material* m_plantMaterial;
	Material* m_materialSelectionCircle;
	Material* m_materialAxisLines;
	Shader* m_shaderLit;
	Shader* m_shaderLitVertexColored;
	Shader* m_shaderUnlit;
	Shader* m_shaderUnlitVertexColored;
};


#endif // _SIMULATION_RENDERER_H_