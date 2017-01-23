#ifndef _SIMULATION_RENDERER_H_
#define _SIMULATION_RENDERER_H_	

#include <graphics/OpenGLIncludes.h>
#include <math/MathLib.h>
#include "ResourceManager.h"
#include "OctTreeRenderer.h"

class SimulationManager;


class SimulationRenderer
{
public:
	SimulationRenderer();
	~SimulationRenderer();

	void Initialize(SimulationManager* simulationManager);

	void Render(const Vector2f& viewPortSize);

private:
	SimulationManager* m_simulationManager;
	ResourceManager	m_resourceManager;
	Renderer		m_renderer;
	OctTreeRenderer	m_octTreeRenderer;

	// Resources.
	Mesh* m_agentMesh;
	Mesh* m_meshSelectionCircle;
	Mesh* m_meshAxisLines;
	Material* m_worldMaterial;
	Material* m_agentMaterial;
	Material* m_materialSelectionCircle;
	Material* m_materialAxisLines;
	Shader* m_shaderLit;
	Shader* m_shaderLitVertexColored;
	Shader* m_shaderUnlit;
	Shader* m_shaderUnlitVertexColored;
};


#endif // _SIMULATION_RENDERER_H_