#ifndef _SIMULATION_RENDERER_H_
#define _SIMULATION_RENDERER_H_	

#include <graphics/OpenGLIncludes.h>
#include <graphics/Graphics.h>
#include <math/MathLib.h>
#include "ResourceManager.h"
#include "OctTreeRenderer.h"
#include "InfoPanel.h"
#include "GraphManager.h"

class SimulationManager;
class Agent;


class SimulationRenderer
{
public:
	SimulationRenderer(SimulationManager* simulationManager);
	~SimulationRenderer();

	// Getters
	inline double GetAverageRenderTime() const { return m_renderTime; }
	inline ResourceManager* GetResourceManager() { return &m_resourceManager; }
	inline OctTreeRenderer* GetOctTreeRenderer() { return &m_octTreeRenderer; }

	void Initialize();
	void OnNewSimulation(Simulation* simulation);

	// Render the simulation display given the canvas size.
	void Render(const Vector2f& canvasSize);


private:
	void LoadResources();

	// Rendering
	void RenderAgentVisionArcs(Agent* agent);
	void RenderAgentVisionStrips(Agent* agent);


private:
	SimulationManager* m_simulationManager;

	ResourceManager	m_resourceManager;
	Renderer		m_renderer; // Used to render 3d meshes
	Graphics		m_graphics; // used to draw 2d shapes
	OctTreeRenderer	m_octTreeRenderer;

	double			m_renderTime; // measured time of render (in seconds)
	Vector2f		m_canvasSize;

	RenderParams	m_skyBoxRenderParams;
	RenderParams	m_simulationRenderParams;
	RenderParams	m_uiRenderParams;

	// Resources
	Font* m_font;
	Texture* m_textureSkyBox;
	Mesh* m_agentMeshHerbivore;
	Mesh* m_agentMeshCarnivore;
	Mesh* m_plantMesh;
	Mesh* m_worldMesh;
	Mesh* m_meshSelectionCircle;
	Mesh* m_meshQuad;
	Mesh* m_meshAxisLines;
	Mesh* m_skyBoxMesh;
	Material* m_worldMaterial;
	Material* m_agentMaterial;
	Material* m_plantMaterial;
	Material* m_materialSelectionCircle;
	Material* m_materialAxisLines;
	Shader* m_shaderLit;
	Shader* m_shaderLitVertexColored;
	Shader* m_shaderUnlit;
	Shader* m_shaderUnlitVertexColored;
	Shader* m_shaderSkyBox;
};


#endif // _SIMULATION_RENDERER_H_