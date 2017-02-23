#include "SimulationRenderer.h"
#include "SimulationManager.h"
#include <simulation/Agent.h>
#include <simulation/Offshoot.h>
#include <simulation/Plant.h>
#include <utilities/Timing.h>
#include <sstream>
#include <iostream>
#include <iomanip>


SimulationRenderer::SimulationRenderer(SimulationManager* simulationManager) :
	m_simulationManager(simulationManager),
	m_octTreeRenderer(simulationManager, &m_resourceManager)
{
}

void SimulationRenderer::LoadResources()
{
	m_resourceManager.SetAssetsPath("../../assets/"); // TODO: this must change when the executable is moved

	//-------------------------------------------------------------------------
	// Load fonts.

	m_font = m_resourceManager.LoadSpriteFont(
		"font", "fonts/font_console.png", 16, 8, 12, 0);

	//-------------------------------------------------------------------------
	// Load shaders.

	m_shaderLit = m_resourceManager.LoadShader("lit",
		"shaders/lit_colored_vs.glsl",
		"shaders/lit_colored_fs.glsl");
	m_shaderLitVertexColored = m_resourceManager.LoadShader("lit_vertex_colored",
		"shaders/lit_vertex_colored_vs.glsl",
		"shaders/lit_vertex_colored_fs.glsl");
	m_shaderUnlit = m_resourceManager.LoadShader("unlit",
		"shaders/unlit_colored_vs.glsl",
		"shaders/unlit_colored_fs.glsl");
	m_shaderUnlitVertexColored = m_resourceManager.LoadShader("unlit_vertex_colored",
		"shaders/unlit_vertex_colored_vs.glsl",
		"shaders/unlit_vertex_colored_fs.glsl");
	
	// Create the default fallback shader used when other shaders have errors.
	Shader* m_defaultShader = new Shader();
	const std::string vertexSource = 
		"#version 330 core\n"
		"in vec3 a_vertPos;\n"
		"uniform mat4 u_mvp;\n"
		"void main() { gl_Position = u_mvp * vec4(a_vertPos, 1); }";
	const std::string fragmentSource = 
		"#version 330 core\n"
		"out vec4 o_color;\n"
		"void main() { o_color = vec4(1,0,1,1); }";
	m_defaultShader->AddStage(vertexSource,
		ShaderType::VERTEX_SHADER, "default_shader_vs");
	m_defaultShader->AddStage(fragmentSource,
		ShaderType::FRAGMENT_SHADER, "default_shader_fs");
	m_defaultShader->CompileAndLink();
	m_renderer.SetDefaultShader(m_defaultShader);
	m_resourceManager.AddShader("default_fallback", m_defaultShader);

	//-------------------------------------------------------------------------
	// Models

	// Agent model.
	m_agentMesh = m_resourceManager.LoadMesh("agent", "models/agent.obj");
	m_agentMaterial = new Material();
	m_agentMaterial->SetColor(Color::BLUE);
	
	// Plant model.
	m_plantMesh = m_resourceManager.LoadMesh("plant", "models/plant.obj");
	m_plantMaterial = new Material();
	m_plantMaterial->SetColor(Color::GREEN);
	// World model (ico-sphere).
	m_worldMesh = m_resourceManager.LoadMesh("icosphere", "models/icosphere.obj");
	m_worldMaterial = new Material();
	m_worldMaterial->SetColor(Color::WHITE);
	
	// TODO: Move this resource creation code somewhere else.

	// Create selection circle mesh.
	{
		std::vector<VertexPos> vertices;
		std::vector<unsigned int> indices;

		for (unsigned int i = 0; i < 40; i++)
		{
			float a = (i / 40.0f) * Math::TWO_PI;
			vertices.push_back(VertexPos(cos(a), 0.0f, sin(a)));
			indices.push_back(i);
		}

		m_meshSelectionCircle = new Mesh();
		m_meshSelectionCircle->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
		m_meshSelectionCircle->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
		m_meshSelectionCircle->SetPrimitiveType(VertexPrimitiveType::LINE_LOOP);
		m_materialSelectionCircle = new Material();
		m_materialSelectionCircle->SetColor(Color::GREEN);
		m_materialSelectionCircle->SetIsLit(false);
	}

	// Create 3-axis lines mesh
	{
		std::vector<VertexPosCol> vertices;
		std::vector<unsigned int> indices;
		vertices.push_back(VertexPosCol(Vector3f(1,0,0),  Color::RED));
		vertices.push_back(VertexPosCol(Vector3f(-1,0,0),  Color::RED));
		vertices.push_back(VertexPosCol(Vector3f(0,1,0),  Color::GREEN));
		vertices.push_back(VertexPosCol(Vector3f(0,-1,0),  Color::GREEN));
		vertices.push_back(VertexPosCol(Vector3f(0,0,1),  Color::BLUE));
		vertices.push_back(VertexPosCol(Vector3f(0,0,-1),  Color::BLUE));
		for (unsigned int i = 0; i < vertices.size(); i++)
			indices.push_back(i);

		m_meshAxisLines = new Mesh();
		m_meshAxisLines->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
		m_meshAxisLines->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
		m_meshAxisLines->SetPrimitiveType(VertexPrimitiveType::LINES);
		m_materialAxisLines= new Material();
		m_materialAxisLines->SetColor(Color::WHITE);
		m_materialAxisLines->SetIsLit(false);
	}
	
	// Add the computationally generated meshes to the resource manager.
	m_resourceManager.AddMesh("axis_lines", m_meshAxisLines);
	m_resourceManager.AddMesh("circle", m_meshSelectionCircle);
	m_resourceManager.AddMaterial("axis_lines", m_materialAxisLines);
	m_resourceManager.AddMaterial("circle", m_materialSelectionCircle);
	m_resourceManager.AddMaterial("world", m_worldMaterial);
}

void SimulationRenderer::Initialize()
{
	LoadResources();

	// Initialize the oct-tree renderer.
	m_octTreeRenderer.Initialize();

	m_graphFitness.SetFont(m_font);
	m_graphFitness.SetTitle("Population Size");
	m_graphFitness.SetXBounds(0, 60);
	m_graphFitness.SetYBounds(0.0f, 10000.0f);
	m_graphFitness.SetYBounds(0.0f, 300);
	m_graphFitness.SetDynamicRange(false);
	m_graphFitness.SetViewport(Viewport(6, 6, 280, 84));
	m_graphFitness.AddGraph("graph", Color::CYAN);
	//m_graphFitness.AddGraph("red", Color::RED);
	//m_graphFitness.AddGraph("green", Color::GREEN);
	//m_graphFitness.AddGraph("blue", Color::BLUE);
}

void SimulationRenderer::OnNewSimulation(Simulation* simulation)
{
	// Notify sub-systems of the new simulation.
	m_octTreeRenderer.OnNewSimulation(simulation);
}

SimulationRenderer::~SimulationRenderer()
{
	// The resource manager will automatically delete our resources.
}

void SimulationRenderer::Render(const Vector2f& canvasSize)
{
	m_canvasSize = canvasSize;

	m_graphics.SetCanvasSize((int) m_canvasSize.x, (int) m_canvasSize.y);
	m_graphics.SetViewportToCanvas();

	double startTime = Time::GetTime();

	Simulation* simulation = m_simulationManager->GetSimulation();
	ICamera* camera = m_simulationManager->GetCameraSystem()->GetActiveCamera();
	Agent* selectedAgent = m_simulationManager->GetSelectedAgent();
	float worldRadius = simulation->GetWorld()->GetRadius();
	Transform3f transform;
	
	Vector3f cameraForward = camera->GetOrientation().GetForward();

	float aspectRatio = canvasSize.x / canvasSize.y;
    glViewport(0, 0, (int) canvasSize.x, (int) canvasSize.y);
	glDisable(GL_SCISSOR_TEST);
	
	m_simulationManager->GetCameraSystem()->SetAspectRatio(aspectRatio);
	
	// Apply render parameters.
	RenderParams renderParams;
	renderParams.EnableDepthTest(true);
	renderParams.EnableDepthBufferWrite(true);
	renderParams.EnableNearFarPlaneClipping(true);
	renderParams.EnableBlend(true);
	renderParams.EnableLineSmooth(false);
	renderParams.EnablePolygonSmooth(false);
	renderParams.EnableCullFace(false);
	renderParams.SetCullFace(CullFace::BACK);
	renderParams.SetFrontFace(FrontFace::CLOCKWISE); // TODO: meshes loaded from file have CCW front-face winding order ??
	renderParams.SetClearBits(ClearBits::COLOR_BUFFER_BIT | ClearBits::DEPTH_BUFFER_BIT);
	renderParams.SetClearColor(Color::BLACK);
	renderParams.SetPolygonMode(PolygonMode::FILL);
	renderParams.SetDepthFunction(CompareFunction::LESS_EQUAL);
	renderParams.SetBlendFunction(BlendFunc::SOURCE_ALPHA, BlendFunc::ONE_MINUS_SOURCE_ALPHA);
	if (m_simulationManager->IsViewWireFrameMode())
		renderParams.SetPolygonMode(PolygonMode::LINE);
	else
		renderParams.SetPolygonMode(PolygonMode::FILL);

	m_renderer.SetRenderParams(renderParams);
	m_renderer.ApplyRenderSettings(true);
	m_renderer.SetCamera(camera);

	// Setup lighting properties.
	if (!m_simulationManager->IsLightingEnabled())
	{
		m_renderer.SetLightColor(Color::BLACK);
		m_renderer.SetAmbientLight(Color::WHITE);
	}
	else
	{
		m_renderer.SetLightColor(Color(255, 220, 132));
		m_renderer.SetAmbientLight(Color(33, 36, 63));
	}

	// Render the world.
	transform.SetIdentity();
	transform.SetScale(worldRadius);
	m_renderer.SetShader(m_shaderLit);
	m_renderer.RenderMesh(m_worldMesh, m_worldMaterial, transform);
	
	// Render all simulation objects.
	Material material;
	material.SetIsLit(true);
	m_renderer.SetShader(m_shaderLit);
	ObjectManager* objectManager = simulation->GetObjectManager();
	for (auto it = objectManager->objects_begin(); it != objectManager->objects_end(); ++it)
	{
		SimulationObject* object = *it;

		// Don't render invisible objects.
		if (!object->IsVisible() && !m_simulationManager->GetShowInvisibleObjects())
			continue;

		Matrix4f modelMatrix = object->GetObjectToWorld() * 
			Matrix4f::CreateScale(object->GetRadius());
		material.SetColor(object->GetColor());
		
		// Render object with the appropriate mesh.
		if (object->IsObjectType<Agent>())
			m_renderer.RenderMesh(m_agentMesh, &material, modelMatrix);
		else if (object->IsObjectType<Plant>())
			m_renderer.RenderMesh(m_plantMesh, &material, modelMatrix);
		else if (object->IsObjectType<Offshoot>())
			m_renderer.RenderMesh(m_plantMesh, &material, modelMatrix);
	}

	// Render agent vision.
	if (m_simulationManager->GetShowAgentVision())
	{
		m_renderer.SetShader(nullptr);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(camera->GetViewProjection().data());

		// If selecting an agent, only show their vision
		if (selectedAgent != nullptr)
		{
			RenderAgentVisionArcs(selectedAgent);
		}
		else
		{
			// Render vision arcs for all agents.
			for (auto it = objectManager->agents_begin();
				it != objectManager->agents_end(); ++it)
			{
				RenderAgentVisionArcs(*it);
			}
		}
	}
	
	// Draw the selection circle.
	if (selectedAgent != nullptr)
	{
		transform.pos = selectedAgent->GetPosition();
		transform.pos.Normalize();
		transform.pos *= worldRadius + 0.001f;
		transform.rot = selectedAgent->GetOrientation();
		transform.SetScale(selectedAgent->GetRadius() * 1.2f);
		m_renderer.SetShader(m_shaderUnlit);
		m_renderer.RenderMesh(m_meshSelectionCircle, m_materialSelectionCircle, transform);
	}
	
	// Render the X/Y/Z axis lines.
	if (m_simulationManager->GetShowAxisLines())
	{
		transform.SetIdentity();
		transform.SetScale(worldRadius * 2.0f);
		m_renderer.SetShader(m_shaderUnlitVertexColored);
		m_renderer.RenderMesh(m_meshAxisLines, m_materialAxisLines, transform);
	}

	// Render the OctTree
	m_octTreeRenderer.RenderOctTree(&m_renderer, simulation->GetOctTree());
	
	// Switch to orthographic mode to render the HUD.
	Matrix4f orthographic = Matrix4f::CreateOrthographic(0.0f, m_canvasSize.x, m_canvasSize.y, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(orthographic.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	m_renderer.SetShader(0);
	orthographic = Matrix4f::CreateOrthographic(0.5f,
		m_canvasSize.x + 0.5f, m_canvasSize.y + 0.5f, 0.5f, -1.0f, 1.0f);
	m_graphics.SetProjection(orthographic);

	if (selectedAgent != nullptr)
	{
		// Render the selected agent's vision strips.
		RenderAgentVisionStrips(selectedAgent);

		// Render brain.
		if (m_simulationManager->GetShowAgentBrain())
			RenderBrain(selectedAgent);
	}
	
	RenderInfoPanel();
	RenderGraphs();
	
	double endTime = Time::GetTime();
	m_renderTime = (endTime - startTime);
}


void SimulationRenderer::RenderAgentVisionArcs(Agent* agent)
{
	Transform3f transform;
	transform.pos = agent->GetPosition();
	transform.rot = agent->GetOrientation();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.GetMatrix().data());

	float angleBetweenEyes = agent->GetAngleBetweenEyes();
	float viewDistance = agent->GetEye(0)->GetViewDistance();
	float fov = agent->GetFieldOfView();
	unsigned int numVertices = 20;

	float red, green, blue;
	float x, z, xPrev, zPrev;
	
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		float t = (((float) i + 0.5f) / (float) (numVertices - 1));
		float angle = (angleBetweenEyes * 0.5f) + (t * fov);
		x = Math::Sin(angle) * viewDistance;
		z = -Math::Cos(angle) * viewDistance;

		if (i > 0)
		{
			// Left eye.
			red   = agent->GetEye(0)->GetSightValue(0, 1.0f - t);
			green = agent->GetEye(0)->GetSightValue(1, 1.0f - t);
			blue  = agent->GetEye(0)->GetSightValue(2, 1.0f - t);
			glColor3f(red, green, blue);
			glVertex3f(-xPrev, 0, zPrev);
			glVertex3f(-x, 0, z);
			glVertex3f(0, 0, 0);

			// Right eye.
			red   = agent->GetEye(1)->GetSightValue(0, t);
			green = agent->GetEye(1)->GetSightValue(1, t);
			blue  = agent->GetEye(1)->GetSightValue(2, t);
			glColor3f(red, green, blue);
			glVertex3f(xPrev, 0, zPrev);
			glVertex3f(x, 0, z);
			glVertex3f(0, 0, 0);
		}

		xPrev = x;
		zPrev = z;
	}
	glEnd();
}


void SimulationRenderer::RenderAgentVisionStrips(Agent* agent)
{
	float stripWidth = 240.0f;
	float stripHeight = 60.0f;
	float separation = 40.0f;
	
	Vector2f center(m_canvasSize.x * 0.5f, m_canvasSize.y - 30 - stripHeight);

	Vector2f stripPos[2];
	stripPos[0].Set(center.x - (separation * 0.5f) - stripWidth, center.y - (stripHeight * 0.5f));
	stripPos[1].Set(center.x + (separation * 0.5f), center.y - (stripHeight * 0.5f));

	Vector3f channelColor;
	Color outlineColor(100, 100, 100);

	// Draw each vision strip.
	for (unsigned int eyeIndex = 0; eyeIndex < 2; eyeIndex++)
	{
		Retina* eye = agent->GetEye(eyeIndex);

		// Render the vision strip for each channel.
		for (unsigned int channel = 0; channel < eye->GetNumChannels(); channel++)
		{
			unsigned int resolution = eye->GetResolution(channel);
			channelColor.SetZero();
			channelColor[channel] = 1.0f;

			Vector2f boxSize(stripWidth / resolution, stripHeight / eye->GetNumChannels());

			// Render each pixel in the sight buffer for this channel.
			for (unsigned int i = 0; i < resolution; i++)
			{
				float sightValue = eye->GetSightValueAtIndex(channel, i);
				Vector2f boxPos = stripPos[eyeIndex] + Vector2f(boxSize.x * i, boxSize.y * channel);
				Vector3f boxFillColor = channelColor * sightValue;

				// Draw the filled box.
				glBegin(GL_QUADS);
					glColor3fv(boxFillColor.data());
					glVertex2f(boxPos.x, boxPos.y);
					glVertex2f(boxPos.x + boxSize.x, boxPos.y);
					glVertex2f(boxPos.x + boxSize.x, boxPos.y + boxSize.y);
					glVertex2f(boxPos.x, boxPos.y + boxSize.y);
				glEnd();
				
				// Draw the box outline.
				glBegin(GL_LINE_LOOP);
					glColor4ubv(outlineColor.data());
					glVertex2f(boxPos.x, boxPos.y);
					glVertex2f(boxPos.x + boxSize.x, boxPos.y);
					glVertex2f(boxPos.x + boxSize.x, boxPos.y + boxSize.y);
					glVertex2f(boxPos.x, boxPos.y + boxSize.y);
				glEnd();
			}
		}
	}
}


void SimulationRenderer::RenderBrain(Agent* agent)
{
	const SimulationConfig& config =
		m_simulationManager->GetSimulation()->GetConfig();
	Brain* brain = agent->GetBrain();

	unsigned int numNeurons = brain->GetNumNeurons();
	unsigned int numInputNeurons = brain->GetNumInputNeurons();
	unsigned int numOutputNeurons = brain->GetNumOutputNeurons();
	unsigned int numOutIntNeurons = numNeurons - numInputNeurons;
	
	// Fit the brain to the width of the window.
	Vector2f cellSize = Vector2f((m_canvasSize.x * 0.7f) / numNeurons);
	cellSize.y = cellSize.x;

	Vector2f center(m_canvasSize.x * 0.5f, 100);
	Vector2f matrixSize(cellSize.x * numNeurons, cellSize.y * numOutIntNeurons);
	Vector2f matrixTopLeft(center.x - (matrixSize.x * 0.5f), 100);
	Vector2f matrixBottomRight = matrixTopLeft + cellSize *
		Vector2f((float) numNeurons, (float) numOutIntNeurons);
	Color outlineColor = Color::WHITE;

	// Draw connection matrix.
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		Neuron neuron = brain->GetNeuron(i);

		for (unsigned int k = neuron.synapsesBegin; k < neuron.synapsesEnd; ++k)
		{
			Synapse synapse = brain->GetSynapse(k);

			Vector2f cellPos;
			cellPos.x = matrixTopLeft.x + (cellSize.x * synapse.neuronFrom);
			cellPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons -
				(synapse.neuronTo - numInputNeurons) - 1));
			
			Vector3f weightColor(0, 0, 0);
			if (synapse.weight > 0.0f)
				weightColor[1] = 1.0f;
			if (synapse.weight < 0.0f)
				weightColor[0] = 1.0f;
			weightColor *= Math::Abs(synapse.weight) / config.brain.maxWeight;
			if (synapse.learningRate == 0.0f)
				weightColor = Vector3f(0.3f, 0.3f, 0.3f);

			// Fill synapse box with color.
			m_graphics.FillRect(cellPos, cellSize, weightColor);
			
			// Draw synapse box outline.
			if (synapse.learningRate != 0.0f)
			{
				weightColor.Set(0, 0, 0);
				if (synapse.learningRate > 0.0f)
					weightColor[1] = 1.0f;
				if (synapse.learningRate < 0.0f)
					weightColor[0] = 1.0f;
				weightColor *= (Math::Abs(synapse.weight) / config.brain.maxWeight) + 0.2f;

				m_graphics.DrawRect(cellPos, cellSize, weightColor);
			}
		}
	}

	// Draw neuron activations.
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		Neuron neuron = brain->GetNeuron(i);
		float activation = brain->GetNeuronActivation(i);

		Vector2f cellPos;
		cellPos.x = matrixTopLeft.x + (cellSize.x * i);
		cellPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1));
			
		Vector3f weightColor(1, 1, 1);

		// Colorize input neurons.
		if (i == 0)
			weightColor.Set(1, 1, 0);
		else if (i == 1)
			weightColor.Set(1, 0, 1);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 2))
			weightColor.Set(1, 0, 0);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 4))
			weightColor.Set(0, 1, 0);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 6))
			weightColor.Set(0, 0, 1);

		// Fill box color.
		m_graphics.FillRect(cellPos, cellSize, weightColor * activation);
		
		// Draw box outline.
		m_graphics.DrawRect(cellPos, cellSize, weightColor * (activation + 0.2f));
	}

	// Draw neuron group seperation lines.
	glBegin(GL_LINES);
	glColor3f(1,1,1);
	glVertex2f(matrixTopLeft.x + cellSize.x * numInputNeurons, matrixTopLeft.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * numInputNeurons, matrixBottomRight.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * (numInputNeurons + numOutputNeurons), matrixTopLeft.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * (numInputNeurons + numOutputNeurons), matrixBottomRight.y);
	glVertex2f(matrixTopLeft.x, matrixBottomRight.y - cellSize.y * numOutputNeurons);
	glVertex2f(matrixBottomRight.x, matrixBottomRight.y - cellSize.y * numOutputNeurons);
	glEnd();
		
	// Draw activations strip outline.
	Vector2f activationsPos;
	activationsPos.x = matrixTopLeft.x;
	activationsPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1));
	m_graphics.DrawRect(
		matrixTopLeft.x - 1,
		matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1)) - 1,
		cellSize.x * numNeurons + 2,
		cellSize.y + 2,
		outlineColor);

	// Draw connection matrix outlines.
	Vector2f one(1, 1);
	Vector2f two(2, 2);
	m_graphics.DrawRect(matrixTopLeft - one, matrixBottomRight -
		matrixTopLeft + two, outlineColor);
}


void SimulationRenderer::RenderGraphs()
{
	Simulation* simulation = m_simulationManager->GetSimulation();
	
	// Setup projection.
	m_graphics.SetProjection(Matrix4f::CreateOrthographic(0.0f,
		m_canvasSize.x, m_canvasSize.y, 0.0f, -1.0f, 1.0f));

	/*
	// Draw graphs.
	Vector2i viewSize(m_canvasSize);
	Vector2i graphSize(280, 84);
	Rect2i graphBox(viewSize.x - graphSize.x - 6, 6,
		graphSize.x, graphSize.y);
		
	std::vector<SimulationStats>& stats = m_simulationManager->GetSimulation()->m_generationStats;
	//m_graphFitness.GetGraph()->ConfigData(&stats.data()->populationSize
	m_graphFitness.SetYBounds(0, 500);
	//m_graphFitness.SetYBounds(0, 1);
	m_graphFitness.GetGraph()->ConfigData(&stats.data()->populationSize,
		(int) stats.size(), sizeof(SimulationStats), 0);
	//m_graphFitness.GetGraph("green")->ConfigData(&stats.data()->avgGeneValue[GenePosition::MUTATION_RATE],
	//	(int) stats.size(), sizeof(SimulationStats), 0);
	//m_graphFitness.GetGraph("blue")->ConfigData(&stats.data()->avgGeneValue[GenePosition::CHILD_COUNT],
	//	(int) stats.size(), sizeof(SimulationStats), 0);
	m_graphFitness.SetXBounds(0, (float) Math::Max(6u, stats.size()));
	m_graphFitness.SetViewport(graphBox);
	m_graphFitness.Draw(m_graphics);

	m_graphics.SetViewportToCanvas();
	*/

	Vector2f graphSize(280, 84);
	Rect2f graphBox(m_canvasSize.x - graphSize.x - 6, 6, graphSize.x, graphSize.y);

	unsigned int graphIndex = (simulation->GetAgeInTicks() / 60) % m_simulationManager->GetGraphManager()->GetNumGraphs();

	GraphInfo* graph = m_simulationManager->GetGraphManager()->GetGraph(graphIndex);
	DrawGraph(m_graphics, *graph, graphBox);
}

void SimulationRenderer::RenderInfoPanel()
{
	Simulation* simulation = m_simulationManager->GetSimulation();
	const SimulationStats& stats = simulation->GetStatistics();
	const SimulationConfig& config = simulation->GetConfig();

	// Setup projection.
	m_graphics.SetProjection(Matrix4f::CreateOrthographic(0.0f,
		m_canvasSize.x, m_canvasSize.y, 0.0f, -1.0f, 1.0f));

	std::string degSymbol = "x";
	degSymbol[0] = (char) 248;

	// Draw info panel for simulation.
	InfoPanel simInfoPanel;
	simInfoPanel.SetTitle("Simulation");
	simInfoPanel.SetFont(m_font);
	simInfoPanel.AddItem("world age").SetValue(simulation->GetAgeInTicks());
	simInfoPanel.AddItem("generation").SetValue(simulation->GetGeneration());
	simInfoPanel.AddItem("season age").SetValue(simulation->GetGenerationAge()).InitBar(Color::MAGENTA, 0u, simulation->GetGenerationDuration());
	simInfoPanel.AddItem("population size").SetValue((int) stats.populationSize).InitBar(Color::CYAN, config.agent.minPreyAgents, config.agent.maxPreyAgents);
	simInfoPanel.AddSeparator();
	simInfoPanel.AddItem("total energy").SetValue(stats.totalEnergy);
	simInfoPanel.AddItem("avg energy").SetValue(stats.avgEnergy);
	simInfoPanel.AddItem("avg energy usage").SetValue(stats.avgEnergyUsage);
	simInfoPanel.AddItem("avg fitness").SetValue(stats.avgFitness);
	simInfoPanel.AddSeparator();
	simInfoPanel.AddItem("avg move amount").SetValue(stats.avgMoveAmount).InitBar(Color::GREEN, 0, 1);
	simInfoPanel.AddItem("avg turn amount").SetValue(stats.avgTurnAmount).InitBar(Color::CYAN, 0, 1);
	Vector2f panelPos = Vector2f(6, 6);
	simInfoPanel.Draw(m_graphics, panelPos);
	
	// Draw info panel for seleced agent.
	Agent* agent = m_simulationManager->GetSelectedAgent();
	if (agent != nullptr)
	{
		Genome* genome = agent->GetGenome();

		std::stringstream text;

		// Set title text.
		text << "Agent " << agent->GetId();
		std::string titleText = text.str();

		// Set color value text as a hexcode
		Color col = Color(agent->GetColor());
		text.str("");
		text << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.r
			<< std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.g
			<< std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int) col.b;
		std::string colorText = text.str();
		
		Color colBlue(0, 106, 255);

		InfoPanel agentInfoPanel;
		agentInfoPanel.SetTitle(titleText);
		agentInfoPanel.SetFont(m_font);
		agentInfoPanel.AddItem("age").SetValue(agent->GetAge()).InitBar(Color::MAGENTA, 0, agent->GetLifeSpan());
		agentInfoPanel.AddItem("energy").SetValue(agent->GetEnergy()).InitBar(Color::YELLOW, 0.0f, agent->GetMaxEnergy());
		agentInfoPanel.AddItem("energy usage").SetValue(agent->GetEnergyUsage()).SetPrecision(4);
		agentInfoPanel.AddItem("fitness").SetValue(agent->GetFitness());
		agentInfoPanel.AddSeparator();
		agentInfoPanel.AddItem("move speed").SetValue(agent->GetMoveSpeed()).InitBar(Color::GREEN, 0.0f, agent->GetMaxMoveSpeed());
		agentInfoPanel.AddItem("turn speed").SetValue(-agent->GetTurnSpeed()).InitBar(Color::CYAN, -agent->GetMaxTurnSpeed(), agent->GetMaxTurnSpeed(), 0.0f);
		agentInfoPanel.AddSeparator();
		agentInfoPanel.AddItem("color").SetValue(text.str()).InitSolidColor(agent->GetColor());
		agentInfoPanel.AddItem("life span").SetValue(agent->GetLifeSpan()).InitBar(Color::MAGENTA, config.genes.minLifeSpan, config.genes.maxLifeSpan);
		agentInfoPanel.AddItem("strength").SetValue(agent->GetStrength()).InitBar(Color::RED, config.genes.minStrength, config.genes.maxStrength);
		agentInfoPanel.AddItem("num children").SetValue(agent->GetDesiredNumChildren()).InitBar(Color::GREEN, config.genes.minChildren, config.genes.maxChildren);
		agentInfoPanel.AddItem("muation rate", "%").SetValue(agent->GetMutationRate() * 100).InitBar(Color::MAGENTA, config.genes.minMutationRate * 100, config.genes.maxMutationRate * 100);
		agentInfoPanel.AddItem("crossover points").SetValue(agent->GetNumCrossoverPoints()).InitBar(Color::YELLOW, config.genes.minCrossoverPoints, config.genes.maxCrossoverPoints);
		agentInfoPanel.AddItem("field of view", degSymbol).SetValue(Math::ToDegrees(agent->GetFieldOfView())).SetPrecision(0).InitBar(Color::GREEN, Math::ToDegrees(config.genes.minFieldOfView), Math::ToDegrees(config.genes.maxFieldOfView));
		agentInfoPanel.AddItem("angle b/w eyes", degSymbol).SetValue(Math::ToDegrees(agent->GetAngleBetweenEyes())).SetPrecision(0).InitBar(Color(255, 128, 0), Math::ToDegrees(config.genes.minAngleBetweenEyes), Math::ToDegrees(config.genes.maxAngleBetweenEyes));
		agentInfoPanel.AddItem("sight distance").SetValue(agent->GetMaxViewDistance()).InitBar(Color::CYAN, config.genes.minSightDistance, config.genes.maxSightDistance);
		agentInfoPanel.AddItem("resolution red").SetValue(agent->GetSightResolution(0)).InitBar(Color::RED, config.genes.minSightResolution, config.genes.maxSightResolution);
		agentInfoPanel.AddItem("resolution green").SetValue(agent->GetSightResolution(1)).InitBar(Color::GREEN, config.genes.minSightResolution, config.genes.maxSightResolution);
		agentInfoPanel.AddItem("resolution blue").SetValue(agent->GetSightResolution(2)).InitBar(colBlue, config.genes.minSightResolution, config.genes.maxSightResolution);

		
		panelPos.y += simInfoPanel.GetSize().y + 10;
		agentInfoPanel.Draw(m_graphics, panelPos);
	}
}

void SimulationRenderer::DrawGraph(Graphics& g, const GraphInfo& graph, const Rect2f& rect)
{
	Simulation* simulation = m_simulationManager->GetSimulation();
	SimulationStats* stats = simulation->m_generationStats.data();
	unsigned int numStats = simulation->m_generationStats.size();

	Color colorBackground	= Color::BLACK;
	Color colorGraphArea	= Color::BLACK;
	Color colorOutline		= Color::WHITE;
	Color colorZeroY		= Color::WHITE;
	Color colorZeroX		= Color::WHITE;
	Color colorGraphMargin	= Color::LIGHT_GRAY;
	Color colorLabels		= Color::WHITE;

	//-------------------------------------------------------------------------
	// Calculate view bounds.

	const GraphRange& range = graph.GetRange();

	Vector2f mins;
	Vector2f maxs;
	for (unsigned int i = 0; i < numStats; ++i)
	{
		float x = (float) i;
		float y = graph.GetData(stats + i);
		if (i == 0 || x < mins.x) mins.x = x;
		if (i == 0 || y < mins.y) mins.y = y;
		if (i == 0 || x > maxs.x) maxs.x = x;
		if (i == 0 || y > maxs.y) maxs.y = y;
	}

	float rangeSpan = maxs.y - mins.y;

	//-------------------------------------------------------------------------
	// Adjust view bounds range for fixed or dynamic clamped ranges.

	if (range.minType == GraphRange::FIXED)
	{
		mins.y = range.minValue;
	}
	else
	{
		mins.y -= range.dynamicPaddingPercent * rangeSpan;
		if (range.minType == GraphRange::DYNAMIC_CLAMPED && mins.y > range.minValue)
			mins.y = range.minValue;
	}
	if (range.maxType == GraphRange::FIXED)
	{
		maxs.y = range.maxValue;
	}
	else
	{
		maxs.y += range.dynamicPaddingPercent * rangeSpan;
		if (range.maxType == GraphRange::DYNAMIC_CLAMPED && maxs.y < range.maxValue)
			maxs.y = range.maxValue;
	}

	//-------------------------------------------------------------------------
	// Determine range label strings and graph area rectangle.

	std::stringstream str;
	str << mins.y;
	std::string labelMin = str.str();

	str.str("");
	str << maxs.y;
	std::string labelMax = str.str();

	int labelMinLength = labelMin.length();
	int labelMaxLength = labelMax.length();
	int labelTextLength = Math::Max(labelMinLength, labelMaxLength);
	
	int padding		= 6;
	int labelWidth	= 8 * labelTextLength;
	int labelHeight	= 12;
	int titleWidth	= graph.GetTitle().length() * 8;
	int titleHeight	= 12;
	
	Rect2f graphRect = rect;
	graphRect.position.x	+= labelWidth + (padding * 2);
	graphRect.position.y	+= padding + titleHeight;
	graphRect.size.x		= rect.size.x - (padding * 3) - labelWidth;
	graphRect.size.y		= rect.size.y - (padding * 2) - titleHeight;

	//-------------------------------------------------------------------------
	
	g.SetViewportToCanvas();
	g.SetCanvasProjection();

	// Fill background
	g.FillRect(rect, colorBackground);

	// Fill graph area background
	g.FillRect(graphRect, colorGraphArea);
	
	// Draw the graph area's left margin.
	g.DrawLine((float) graphRect.position.x,
				(float) graphRect.position.y,
				(float) graphRect.position.x,
				(float) graphRect.position.y + graphRect.size.y,
				colorGraphMargin);
	
	// Draw tick marks at the top and bottom of the graph area's left margin.
	int tickMarkWidth = 6;
	g.DrawLine((float) graphRect.position.x - (tickMarkWidth / 2),
				(float) graphRect.position.y,
				(float) graphRect.position.x + (tickMarkWidth / 2),
				(float) graphRect.position.y,
				colorGraphMargin);
	g.DrawLine((float) graphRect.position.x - (tickMarkWidth / 2),
				(float) graphRect.position.y + graphRect.size.y,
				(float) graphRect.position.x + (tickMarkWidth / 2),
				(float) graphRect.position.y + graphRect.size.y,
				colorGraphMargin);
	
	//-------------------------------------------------------------------------
	// Draw labels.

	// Draw the y-axis labels.
	g.DrawString(m_font, labelMax,
		graphRect.position.x - padding,
		graphRect.position.y,
		colorLabels, TextAlign::MIDDLE_RIGHT);
	g.DrawString(m_font, labelMin,
		graphRect.position.x - padding,
		graphRect.position.y + graphRect.size.y,
		colorLabels, TextAlign::MIDDLE_RIGHT);

	// Draw title
	g.DrawString(m_font, graph.GetTitle(),
		rect.position.x + (float) (rect.size.x / 2),
		rect.position.y + (float) padding,
		colorLabels, TextAlign::TOP_CENTER);

	//-------------------------------------------------------------------------
	
	// Setup projection for graph view bounds.
	Matrix4f viewBoundsProjection;
	viewBoundsProjection.InitOrthographic(mins.x, maxs.x, mins.y, maxs.y, -1, 1);
	g.SetViewport(Rect2i((int) graphRect.position.x,
		(int) graphRect.position.y, (int) graphRect.size.x, (int) graphRect.size.y), true);
	g.SetProjection(viewBoundsProjection);
	
	// Draw the graph line.
	glBegin(GL_LINE_STRIP);
	glColor4ubv(graph.GetColor().data());
	for (unsigned int i = 0; i < numStats; ++i)
	{
		float x = (float) i;
		float y = graph.GetData(stats + i);
		if (i == 0 && x < mins.x) mins.x = x;
		if (i == 0 && y < mins.y) mins.y = y;
		if (i == 0 && x > maxs.x) maxs.x = x;
		if (i == 0 && y > maxs.y) maxs.y = y;
		glVertex2f(x, y);
	}
	glEnd();

	g.SetViewportToCanvas();
	g.SetCanvasProjection();
	
	//-------------------------------------------------------------------------

	// Draw graph outline
	//g.DrawRect(graphRect, colorOutline);

	// Draw graph outline
	g.DrawRect(rect, colorOutline);
}


