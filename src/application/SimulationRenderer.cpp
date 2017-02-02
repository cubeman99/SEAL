#include "SimulationRenderer.h"
#include "SimulationManager.h"



SimulationRenderer::SimulationRenderer()
{
}

void SimulationRenderer::Initialize(SimulationManager* simulationManager)
{
	m_simulationManager = simulationManager;
	m_resourceManager.SetAssetsPath("../../assets/"); // TODO: magic string

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
		"void main()\n"
		"{\n"
			"gl_Position = u_mvp * vec4(a_vertPos, 1);\n"
		"}";
	const std::string fragmentSource = 
		"#version 330 core\n"
		"out vec4 o_color;\n"
		"void main()\n"
		"{\n"
			"o_color = vec4(1,0,1,1);\n" // default color = magenta
		"}";
	m_defaultShader->AddStage(vertexSource, ShaderType::VERTEX_SHADER, "default_shader_vs");
	m_defaultShader->AddStage(fragmentSource, ShaderType::FRAGMENT_SHADER, "default_shader_fs");
	m_defaultShader->CompileAndLink();
	m_renderer.SetDefaultShader(m_defaultShader);

	m_octTreeRenderer.Initialize(&m_resourceManager, simulationManager);

	// TODO: Move this resource creation code somewhere else.

	// Create agent mesh.
	{
		float h = 0.3f;
		Vector3f v1(0, h, -1);			// top front
		Vector3f v2(0.7f, h, 0.7f);		// top back right
		Vector3f v3(-0.7f, h, 0.7f);	// top back left
		Vector3f v4(0, 0, -1);			// bottom front
		Vector3f v5(0.7f, 0, 0.7f);		// bottom back right
		Vector3f v6(-0.7f, 0, 0.7f);	// bottom back left
		Vector3f n1 = Vector3f::UP;							// top
		Vector3f n2 = Vector3f(1.7f, 0, 0.7f).Normalize();	// right
		Vector3f n3 = Vector3f::BACK;						// back
		Vector3f n4 = Vector3f(-1.7f, 0, 0.7f).Normalize(); // left

		VertexPosNorm vertices[] = {
			VertexPosNorm(v1, n1), // Top
			VertexPosNorm(v2, n1),
			VertexPosNorm(v3, n1),
			VertexPosNorm(v2, n2), // Right
			VertexPosNorm(v1, n2),
			VertexPosNorm(v4, n2),
			VertexPosNorm(v5, n2),
			VertexPosNorm(v3, n3), // Back
			VertexPosNorm(v2, n3),
			VertexPosNorm(v5, n3),
			VertexPosNorm(v6, n3),
			VertexPosNorm(v1, n4), // Left
			VertexPosNorm(v3, n4),
			VertexPosNorm(v6, n4),
			VertexPosNorm(v4, n4),
		};
		unsigned int indices[] = {
			0, 1, 2,
			3, 4, 5, 3, 5, 6,
			7, 8, 9, 7, 9, 10,
			11, 12, 13, 11, 13, 14,
		};

		//m_agentMesh = new Mesh();
		//m_agentMesh->GetVertexData()->BufferVertices(15, vertices);
		//m_agentMesh->GetIndexData()->BufferIndices(21, indices);

		//m_agentMaterial = new Material();
		//m_agentMaterial->SetColor(Color::BLUE);
	}

	m_agentMesh = m_resourceManager.LoadMesh("agent", "models/ae86.obj");
	m_agentMesh->SetTransformMatrix(Matrix4f::CreateTranslation(0, 0.4f, 0));
	m_agentMaterial = new Material();
	m_agentMaterial->SetColor(Color::BLUE);
	
	// Create plant mesh.
	{
		float h = 0.1f * 10;
		float l = 1.0f * 10;
		Vector3f v1(-l, h, -l);			// top front left
		Vector3f v2( l, h, -l);			// top front right
		Vector3f v3(-l, h, l);			// top back left
		Vector3f v4( l, h, l);			// top back right
		Vector3f v5( -l, 0, -l);		// bottom front left
		Vector3f v6( l, 0, -l);			// bottom front right
		Vector3f v7( -l, 0, l);			// bottom back left
		Vector3f v8( l, 0, l);			// bottom back right

		Vector3f n1 = Vector3f::UP;		// top
		Vector3f n2 = Vector3f::RIGHT;	// right
		Vector3f n3 = Vector3f::BACK;	// back
		Vector3f n4 = Vector3f::LEFT;	// left
		Vector3f n5 = Vector3f::FORWARD;// front

		VertexPosNorm vertices[] = {			// is this right?
			VertexPosNorm(v1, n1), // Top
			VertexPosNorm(v2, n1),
			VertexPosNorm(v3, n1),
			VertexPosNorm(v4, n1),
			VertexPosNorm(v1, n5), // Front
			VertexPosNorm(v2, n5),
			VertexPosNorm(v5, n5),
			VertexPosNorm(v6, n5),
			VertexPosNorm(v3, n3), // Back
			VertexPosNorm(v4, n3),
			VertexPosNorm(v7, n3),
			VertexPosNorm(v8, n3),
			VertexPosNorm(v1, n4), // Left
			VertexPosNorm(v3, n4),
			VertexPosNorm(v5, n4),
			VertexPosNorm(v7, n4),
			VertexPosNorm(v2, n2), // Right
			VertexPosNorm(v4, n2),
			VertexPosNorm(v6, n2),
			VertexPosNorm(v8, n2),
		};
		unsigned int indices[] = {			// this part I really wasn't sure
			0, 1, 2, 3, 0,
		};

		m_plantMesh = new Mesh();
		m_plantMesh->GetVertexData()->BufferVertices(20, vertices);
		m_plantMesh->GetIndexData()->BufferIndices(5, indices); // <<<<<<<<<<<< Adjust indice count

		m_plantMaterial = new Material();
		m_plantMaterial->SetColor(Color::GREEN);
	}

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

	m_worldMaterial = new Material();
	m_worldMaterial->SetColor(Color::WHITE);

	m_resourceManager.AddMesh("axis_lines", m_meshAxisLines);
	m_resourceManager.AddMesh("circle", m_meshSelectionCircle);
	m_resourceManager.AddMesh("agent", m_agentMesh);
	m_resourceManager.AddMesh("plant", m_plantMesh);
	
	m_resourceManager.AddMaterial("axis_lines", m_materialAxisLines);
	m_resourceManager.AddMaterial("circle", m_materialSelectionCircle);
	m_resourceManager.AddMaterial("agent", m_agentMaterial);
	m_resourceManager.AddMaterial("plant", m_plantMaterial);
	m_resourceManager.AddMaterial("world", m_worldMaterial);

}

SimulationRenderer::~SimulationRenderer()
{
	// The resource manager will automatically delete our resources.
}

void SimulationRenderer::Render(const Vector2f& viewPortSize)
{
	Simulation* simulation = m_simulationManager->GetSimulation();
	ICamera* camera = m_simulationManager->GetCameraSystem()->GetActiveCamera();
	Agent* selectedAgent = m_simulationManager->GetSelectedAgent();
	float worldRadius = simulation->GetWorld()->GetRadius();
	Transform3f transform;

	float aspectRatio = viewPortSize.x / viewPortSize.y;
    glViewport(0, 0, (int) viewPortSize.x, (int) viewPortSize.y);
	
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
	m_renderer.SetShader(m_shaderLitVertexColored);
	m_renderer.RenderMesh(simulation->GetWorld()->GetMesh(), m_worldMaterial, transform);
	
	// Render the plants (meaning their offshoots)
	PlantSystem* plantSystem = simulation->GetPlantSystem();
	float plantRadius = 0.016f; // TODO: magic number plant radius?
	float plantOffset = worldRadius - Math::Sqrt((worldRadius * worldRadius) - (plantRadius * plantRadius));
	m_renderer.SetShader(m_shaderLit);
	for (auto it = plantSystem->plants_begin(); it != plantSystem->plants_end(); it++)
	{
		Plant* plant = *it;

		for (auto it = plant->offshoots_begin(); it != plant->offshoots_end(); it++)
		{
			Offshoot* offshoot = *it;
			transform.pos = offshoot->GetPosition();
			transform.pos.Normalize();
			transform.pos *= worldRadius - plantOffset;
			transform.rot = offshoot->GetOrientation();
			transform.SetScale(plantRadius);
			m_renderer.RenderMesh(m_agentMesh, m_agentMaterial, transform); // TODO: finish plant "box" and change this line
		}
	}

	// Render the agents.
	AgentSystem* agentSystem = simulation->GetAgentSystem();
	float agentRadius = 0.016f; // TODO: magic number agent radius.
	float agentOffset = worldRadius - Math::Sqrt((worldRadius * worldRadius) - (agentRadius * agentRadius));
		m_renderer.SetShader(m_shaderLit);
	for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	{
		Agent* agent = *it;
		transform.pos = agent->GetPosition();
		transform.pos.Normalize();
		transform.pos *= worldRadius - agentOffset;
		transform.rot = agent->GetOrientation();
		transform.SetScale(agentRadius);
		m_renderer.RenderMesh(m_agentMesh, m_agentMaterial, transform);
	}
	
	// Draw the selection circle.
	if (selectedAgent != nullptr)
	{	transform.pos = selectedAgent->GetPosition();
		transform.pos.Normalize();
		transform.pos *= worldRadius + 0.001f;
		transform.rot = selectedAgent->GetOrientation();
		transform.SetScale(agentRadius * 1.2f);
		m_renderer.SetShader(m_shaderUnlit);
		m_renderer.RenderMesh(m_meshSelectionCircle, m_materialSelectionCircle, transform);
	}
	
	// Render the X/Y/Z axis lines.
	transform.SetIdentity();
	transform.SetScale(worldRadius * 2.0f);
	m_renderer.SetShader(m_shaderUnlitVertexColored);
	m_renderer.RenderMesh(m_meshAxisLines, m_materialAxisLines, transform);
	
	// Render the OctTree
	m_octTreeRenderer.RenderOctTree(&m_renderer, simulation->GetOctTree());

}



