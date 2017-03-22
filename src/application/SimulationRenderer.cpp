#include "SimulationRenderer.h"
#include "SimulationManager.h"
#include "graphics/ParticleSystem.h"
#include <simulation/Agent.h>
#include <simulation/Offshoot.h>
#include <simulation/Plant.h>
#include <utilities/Timing.h>
#include <graphics/OpenGLIncludes.h>
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
	// Load textures.
	
	// NOTE: default filter params will break this because of mip maps
	TextureParams texParams;
	texParams.SetMinFilter(TextureFilterOptions::LINEAR);
	texParams.SetMagFilter(TextureFilterOptions::LINEAR);
	texParams.SetMipMapFilter(TextureFilterOptions::NONE);

	// Skybox
	std::string skyBoxFileNames[6] = {
		"textures/starbox_right1.png",
		"textures/starbox_left2.png",
		"textures/starbox_top3.png",
		"textures/starbox_bottom4.png",
		"textures/starbox_front5.png",
		"textures/starbox_back6.png",
	};
	m_textureSkyBox = m_resourceManager.LoadCubeMapTexture(
		"skybox", skyBoxFileNames, texParams);

	// TODO: load particle textures here
	m_textureHeart = m_resourceManager.LoadTexture("particle_heart",
		"textures/particle_heart.png", texParams);

	//-------------------------------------------------------------------------
	// Load fonts.

	m_font = m_resourceManager.LoadSpriteFont(
		"font", "fonts/font_console.png", 16, 8, 12, 0);

	//-------------------------------------------------------------------------
	// Load shaders.

	m_shaderLit = m_resourceManager.LoadShader("lit",
		"shaders/lit_colored_vs.glsl",
		"shaders/lit_colored_fs.glsl");
	m_shaderLitTextured = m_resourceManager.LoadShader("lit_textured",
		"shaders/lit_textured_vs.glsl",
		"shaders/lit_textured_fs.glsl");
	m_shaderLitVertexColored = m_resourceManager.LoadShader("lit_vertex_colored",
		"shaders/lit_vertex_colored_vs.glsl",
		"shaders/lit_vertex_colored_fs.glsl");
	m_shaderUnlit = m_resourceManager.LoadShader("unlit",
		"shaders/unlit_colored_vs.glsl",
		"shaders/unlit_colored_fs.glsl");
	m_shaderUnlitVertexColored = m_resourceManager.LoadShader("unlit_vertex_colored",
		"shaders/unlit_vertex_colored_vs.glsl",
		"shaders/unlit_vertex_colored_fs.glsl");
	m_shaderSkyBox = m_resourceManager.LoadShader("skybox",
		"shaders/skybox_vs.glsl",
		"shaders/skybox_fs.glsl");
	
	// Create the default fallback shader used when other shaders have errors.
	// This shader will set the clip position and color things solid magenta.
	Shader* m_defaultShader = m_resourceManager.CreateShader("default_fallback");
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

	//-------------------------------------------------------------------------
	// Loaded Models

	// Agent model.
	m_agentMeshCarnivore = m_resourceManager.LoadMesh("carnivore", "models/agent.obj");
	m_agentMeshHerbivore = m_resourceManager.LoadMesh("herbivore", "models/herbivore.obj");
	m_agentMaterial = m_resourceManager.CreateMaterial("agent");
	m_agentMaterial->SetColor(Color::BLUE);
	m_agentMeshes[SPECIES_CARNIVORE] = m_agentMeshCarnivore;
	m_agentMeshes[SPECIES_HERBIVORE] = m_agentMeshHerbivore;

	// Plant model.
	m_plantMesh = m_resourceManager.LoadMesh("plant", "models/plant.obj");
	m_plantMaterial = m_resourceManager.CreateMaterial("plant");
	m_plantMaterial->SetColor(Color::GREEN);

	// World model (ico-sphere).
	m_worldMesh = m_resourceManager.LoadMesh("icosphere", "models/icosphere.obj");
	m_worldMaterial = m_resourceManager.CreateMaterial("world");
	m_worldMaterial->SetColor(Color::WHITE);
	
	// Skybox inverted-cube model.
	m_skyBoxMesh = m_resourceManager.LoadMesh("skybox", "models/skybox.obj");

	//-------------------------------------------------------------------------
	// Prodedural models

	// Textured, unit quad for rendering billboards
	{
		VertexPosTexNorm vertices[4] {
			VertexPosTexNorm(Vector3f(-0.5f,  0.5f, 0), Vector2f(0,0), Vector3f::UNITZ),
			VertexPosTexNorm(Vector3f(-0.5f, -0.5f, 0), Vector2f(0,1), Vector3f::UNITZ),
			VertexPosTexNorm(Vector3f( 0.5f, -0.5f, 0), Vector2f(1,1), Vector3f::UNITZ),
			VertexPosTexNorm(Vector3f( 0.5f,  0.5f, 0), Vector2f(1,0), Vector3f::UNITZ),
		};
		unsigned int indices[4] = {
			0, 1, 2, 3
		};
		
		m_meshQuad = m_resourceManager.CreateMesh("quad");
		m_meshQuad->GetVertexData()->BufferVertices(4, vertices);
		m_meshQuad->GetIndexData()->BufferIndices(4, indices);
		m_meshQuad->SetPrimitiveType(VertexPrimitiveType::TRIANGLE_FAN);
	}

	// Selection circle
	{
		std::vector<VertexPos> vertices;
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < 40; i++)
		{
			float a = (i / 40.0f) * Math::TWO_PI;
			vertices.push_back(VertexPos(Math::Cos(a), 0.0f, Math::Sin(a)));
			indices.push_back(i);
		}

		m_meshSelectionCircle = m_resourceManager.CreateMesh("circle");
		m_meshSelectionCircle->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
		m_meshSelectionCircle->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
		m_meshSelectionCircle->SetPrimitiveType(VertexPrimitiveType::LINE_LOOP);
		m_materialSelectionCircle = m_resourceManager.CreateMaterial("circle");
		m_materialSelectionCircle->SetColor(Color::GREEN);
		m_materialSelectionCircle->SetIsLit(false);
	}

	// Vertex-colored, 3-axis lines
	{
		VertexPosCol vertices[6] {
			VertexPosCol(Vector3f(1,0,0),  Color::RED),
			VertexPosCol(Vector3f(-1,0,0),  Color::RED),
			VertexPosCol(Vector3f(0,1,0),  Color::GREEN),
			VertexPosCol(Vector3f(0,-1,0),  Color::GREEN),
			VertexPosCol(Vector3f(0,0,1),  Color::BLUE),
			VertexPosCol(Vector3f(0,0,-1),  Color::BLUE)
		};
		unsigned int indices[] = {
			0, 1, 2, 3, 4, 5
		};

		m_meshAxisLines = m_resourceManager.CreateMesh("axis_lines");
		m_meshAxisLines->GetVertexData()->BufferVertices(6, vertices);
		m_meshAxisLines->GetIndexData()->BufferIndices(6, indices);
		m_meshAxisLines->SetPrimitiveType(VertexPrimitiveType::LINES);
		m_materialAxisLines = m_resourceManager.CreateMaterial("axis_lines");
		m_materialAxisLines->SetColor(Color::WHITE);
		m_materialAxisLines->SetIsLit(false);
	}
}

void SimulationRenderer::Initialize()
{
	LoadResources();

	// Initialize render parameters.
	m_simulationRenderParams.EnableDepthTest(true);
	m_simulationRenderParams.EnableDepthBufferWrite(true);
	m_simulationRenderParams.EnableNearFarPlaneClipping(true);
	m_simulationRenderParams.EnableBlend(true);
	m_simulationRenderParams.EnableLineSmooth(false);
	m_simulationRenderParams.EnablePolygonSmooth(false);
	m_simulationRenderParams.EnableCullFace(true);
	m_simulationRenderParams.SetCullFace(CullFace::BACK);
	m_simulationRenderParams.SetFrontFace(FrontFace::COUNTER_CLOCKWISE);
	m_simulationRenderParams.SetClearBits(ClearBits::COLOR_BUFFER_BIT |
		ClearBits::DEPTH_BUFFER_BIT);
	m_simulationRenderParams.SetClearColor(Color::BLACK);
	m_simulationRenderParams.SetPolygonMode(PolygonMode::FILL);
	m_simulationRenderParams.SetDepthFunction(CompareFunction::LESS_EQUAL);
	m_simulationRenderParams.SetBlendFunction(BlendFunc::SOURCE_ALPHA,
		BlendFunc::ONE_MINUS_SOURCE_ALPHA);

	m_heatMapRenderParams = m_simulationRenderParams;

	m_skyBoxRenderParams = m_simulationRenderParams;
	m_skyBoxRenderParams.EnableDepthBufferWrite(false);
	m_skyBoxRenderParams.EnableDepthTest(false);

	m_uiRenderParams = m_simulationRenderParams;
	m_uiRenderParams.EnableDepthTest(false);
	m_uiRenderParams.EnableCullFace(false);
	m_uiRenderParams.SetPolygonMode(PolygonMode::FILL);

	// Initialize the oct-tree renderer.
	m_octTreeRenderer.Initialize();
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
	double startTime = Time::GetTime();

	m_canvasSize = canvasSize;

	Simulation* simulation = m_simulationManager->GetSimulation();
	ICamera* camera = m_simulationManager->
		GetCameraSystem()->GetActiveCamera();
	Agent* selectedAgent = m_simulationManager->GetSelectedAgent();
	float worldRadius = simulation->GetWorld()->GetRadius();
	Transform3f transform;
	int uniformLocation = -1;
	
	float aspectRatio = canvasSize.x / canvasSize.y;
	m_simulationManager->GetCameraSystem()->SetAspectRatio(aspectRatio);
	
	//-------------------------------------------------------------------------
	// Setup render parameters.
	
	if (m_simulationManager->IsViewWireFrameMode())
		m_simulationRenderParams.SetPolygonMode(PolygonMode::LINE);
	else
		m_simulationRenderParams.SetPolygonMode(PolygonMode::FILL);

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
	
	//-------------------------------------------------------------------------
	// Clear the screen.
	
    glViewport(0, 0, (int) canvasSize.x, (int) canvasSize.y);
	glDisable(GL_SCISSOR_TEST);
	
	m_renderer.SetRenderParams(m_simulationRenderParams);
	m_renderer.ApplyRenderSettings(true);

	//-------------------------------------------------------------------------
	// Draw the skybox.
	
	m_renderer.SetRenderParams(m_skyBoxRenderParams);
	m_renderer.ApplyRenderSettings(false);
	m_renderer.SetShader(m_shaderSkyBox);

	// Bind the skybox texture.
	if (m_shaderSkyBox->GetUniformLocation("u_texture", uniformLocation))
		glUniform1i(uniformLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureSkyBox->GetGLTextureId());
	
	transform.SetIdentity();
	transform.SetPosition(m_renderer.GetCamera()->GetViewPosition());
	m_renderer.RenderMesh(m_skyBoxMesh, m_worldMaterial, transform);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	//-------------------------------------------------------------------------
	// Render the simulation.
	
	m_renderer.SetRenderParams(m_simulationRenderParams);
	m_renderer.ApplyRenderSettings(false);

	// Draw the world sphere.
	transform.SetIdentity();
	transform.SetScale(worldRadius);
	m_renderer.SetShader(m_shaderLit);
	m_renderer.RenderMesh(m_worldMesh, m_worldMaterial, transform);
	
	// Draw all simulation objects.
	Material material;
	material.SetIsLit(true);
	m_renderer.SetShader(m_shaderLit);
	ObjectManager* objectManager = simulation->GetObjectManager();
	for (auto it = objectManager->objects_begin();
		it != objectManager->objects_end(); ++it)
	{
		SimulationObject* object = *it;

		// Don't render invisible objects.
		if (!object->IsVisible() &&
			!m_simulationManager->GetShowInvisibleObjects())
			continue;

		Matrix4f modelMatrix = object->GetObjectToWorld() * 
			Matrix4f::CreateScale(object->GetRadius());
		material.SetColor(object->GetColor());
		
		// Render object with the appropriate mesh.
		if (object->IsObjectType<Agent>())
		{
			Agent* agent = (Agent*) object;
			if (agent->GetSpecies() == SPECIES_HERBIVORE)
				m_renderer.RenderMesh(m_agentMeshHerbivore, &material, modelMatrix);
			else
				m_renderer.RenderMesh(m_agentMeshCarnivore, &material, modelMatrix);
		}
		else if (object->IsObjectType<Plant>())
			m_renderer.RenderMesh(m_plantMesh, &material, modelMatrix);
		else if (object->IsObjectType<Offshoot>())
			m_renderer.RenderMesh(m_plantMesh, &material, modelMatrix);
	}

	// Draw particles.
	std::vector<Particle*> particles = simulation->GetParticleSystem()->GetParticles();
	m_renderer.SetShader(m_shaderLitTextured);
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i]->GetInUse())
		{
			// Make the quad-model face the camera.
			Matrix4f modelMatrix =
				Matrix4f::CreateTranslation(particles[i]->GetPosition()) *
				Matrix4f::CreateRotation(camera->GetOrientation()) *
				Matrix4f::CreateScale(particles[i]->GetRadius()); 

			material.SetColor(particles[i]->GetColor());
			material.SetTexture(m_textureHeart);
			m_renderer.RenderMesh(m_meshQuad, &material, modelMatrix);
		}
	}

	// Draw heat-map overlay, colorizing agents by some value.
	RenderHeatMapOverlay();

	// Draw agent vision.
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
		m_renderer.RenderMesh(m_meshSelectionCircle,
			m_materialSelectionCircle, transform);
	}
	
	// Draw the X/Y/Z axis lines.
	if (m_simulationManager->GetShowAxisLines())
	{
		transform.SetIdentity();
		transform.SetScale(worldRadius * 2.0f);
		m_renderer.SetShader(m_shaderUnlitVertexColored);
		m_renderer.RenderMesh(m_meshAxisLines, m_materialAxisLines, transform);
	}

	// Draw the OctTree.
	m_octTreeRenderer.RenderOctTree(&m_renderer, simulation->GetOctTree());
	
	
	//-------------------------------------------------------------------------
	// Render UI overlay.
	
	m_renderer.SetRenderParams(m_uiRenderParams);
	m_renderer.ApplyRenderSettings(false);
	m_graphics.SetupCanvas2D((int) m_canvasSize.x, (int) m_canvasSize.y);

	if (selectedAgent != nullptr)
	{
		// Render the selected agent's vision strips.
		RenderAgentVisionStrips(selectedAgent);

		// Render brain.
		if (m_simulationManager->GetShowAgentBrain())
		{
			Rect2f brainBounds(Vector2f::ZERO, m_canvasSize);
			m_simulationManager->GetDiagramDrawer()->
				DrawBrainMatrix(m_graphics, selectedAgent, brainBounds);
		}
	}

	//-------------------------------------------------------------------------
	
	double endTime = Time::GetTime();
	m_renderTime = (endTime - startTime);
}


//-----------------------------------------------------------------------------
// Private rendering methods
//-----------------------------------------------------------------------------

// Draw heat-map overlay, colorizing agents by some value.
void SimulationRenderer::RenderHeatMapOverlay()
{
	if (m_simulationManager->GetActiveHeatMapIndex() < 0)
		return;

	Simulation* simulation = m_simulationManager->GetSimulation();
	ObjectManager* objectManager = simulation->GetObjectManager();
	HeatMapInfo* heatMap = m_simulationManager->GetHeatMapManager()->
		GetHeatMap(m_simulationManager->GetActiveHeatMapIndex());

	//-------------------------------------------------------------------------
	// Calculate value range.

	float maxValue = FLT_MIN;
	float minValue = FLT_MAX;
	for (auto it = objectManager->agents_begin();
		it != objectManager->agents_end(); ++it)
	{
		float value = heatMap->GetData(*it);
		if (value < minValue)
			minValue = value;
		if (value > maxValue)
			maxValue = value;
	}
	heatMap->GetRange().AdjustValueRange(minValue, maxValue);

	//-------------------------------------------------------------------------
	// Darken the screen.

	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(0);
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBegin(GL_TRIANGLE_FAN);
			glColor4f(0, 0, 0, 0.8f);
			glVertex2f(-1, 1);
			glVertex2f(-1, -1);
			glVertex2f(1, -1);
			glVertex2f(1, 1);
		glEnd();
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(1);

	//-------------------------------------------------------------------------
	// Draw colorized agents.
		
	Material material;
	material.SetIsLit(true);
	material.SetTexture(nullptr);

	m_renderer.SetRenderParams(m_heatMapRenderParams);
	m_renderer.ApplyRenderSettings(false);
	m_renderer.SetShader(m_shaderUnlit);
	
	SpeciesFilter speciesFilter = m_simulationManager->GetHeatMapSpeciesFilter();

	for (auto it = objectManager->agents_begin();
		it != objectManager->agents_end(); ++it)
	{
		Agent* agent = *it;
		
		// Check the species filter for this agent.
		if ((speciesFilter == SPECIES_FILTER_ONLY_HERBIVORES &&
			agent->GetSpecies() != SPECIES_HERBIVORE) ||
			(speciesFilter == SPECIES_FILTER_ONLY_CARNIVORES &&
			agent->GetSpecies() != SPECIES_CARNIVORE))
		{
			continue;
		}

		// Colorize based on the normalized value for this agent.
		float value = heatMap->GetData(agent);
		value = (value - minValue) / (maxValue - minValue);
		material.SetColor(Color::Lerp(Color::BLACK, heatMap->GetColor(), value));

		Matrix4f modelMatrix = agent->GetObjectToWorld() * 
			Matrix4f::CreateScale(agent->GetRadius());
		
		// Draw outline.
		glCullFace(GL_FRONT);
		glDepthMask(0);
		material.SetColor(heatMap->GetColor());
		m_renderer.RenderMesh(m_agentMeshes[agent->GetSpecies()],
			&material, modelMatrix * Matrix4f::CreateScale(1.1f));
		glDepthMask(1);
		glCullFace(GL_BACK);

		// Fill agent with its value color.
		material.SetColor(Color::Lerp(Color::BLACK, heatMap->GetColor(), value));
		m_renderer.RenderMesh(m_agentMeshes[agent->GetSpecies()],
			&material, modelMatrix);
	}
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
	float alpha = 0.3f;
	
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
			glColor4f(red, green, blue, alpha);
			glVertex3f(-xPrev, 0, zPrev);
			glVertex3f(-x, 0, z);
			glVertex3f(0, 0, 0);

			// Right eye.
			red   = agent->GetEye(1)->GetSightValue(0, t);
			green = agent->GetEye(1)->GetSightValue(1, t);
			blue  = agent->GetEye(1)->GetSightValue(2, t);
			glColor4f(red, green, blue, alpha);
			glVertex3f(xPrev, 0, zPrev);
			glVertex3f(0, 0, 0);
			glVertex3f(x, 0, z);
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
