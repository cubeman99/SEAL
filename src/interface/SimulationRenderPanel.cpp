#include "SimulationRenderPanel.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Simulation.h>
#include "SimulationWindow.h"

	
bool OpenAndGetContents(const std::string& fileName, std::string& out)
{
	FILE* m_file = fopen(fileName.c_str(), "r");

	if (m_file == nullptr)
		return false;

	if (fseek(m_file, 0, SEEK_END) != 0)
		return false;

	long fileSize = ftell(m_file);
	if (fileSize < 0)
		return false;

	rewind(m_file);

	const size_t fileSizeInclNull = fileSize + 1;
	char* buffer = new char[fileSizeInclNull];

	size_t result = fread(buffer, 1, fileSizeInclNull, m_file);
	if (result == 0)
	{
		delete buffer;
		return false;
	}

	buffer[result] = '\0';
	out.assign(buffer, buffer + result);

	delete buffer;
	return true;
}

Shader* LoadShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSource;
	std::string fragmentSource;
	
	if (!OpenAndGetContents(vertexPath, vertexSource))
	{
		wxMessageBox("Error loading vertex shader " + vertexPath, "Shader Error", wxICON_WARNING);
		return false;
	}
	if (!OpenAndGetContents(fragmentPath, fragmentSource))
	{
		wxMessageBox("Error loading fragment shader " + fragmentPath, "Shader Error", wxICON_WARNING);
		return false;
	}

	Shader* shader = new Shader();
	
	// Add the two stages.
	shader->AddStage(vertexSource, ShaderType::VERTEX_SHADER, vertexPath);
	shader->AddStage(fragmentSource, ShaderType::FRAGMENT_SHADER, fragmentPath);

	// Compile and link the shader.
	ShaderCompileError compileError;
	if (!shader->CompileAndLink(&compileError))
	{
		wxMessageBox(compileError.GetMessage(), "Shader Error", wxICON_WARNING);

		delete shader;
		return nullptr;
	}

	return shader;
}


// ----------------------------------------------------------------------------
// SimulationRenderPanel
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SimulationRenderPanel, wxGLCanvas)
    EVT_KEY_DOWN(SimulationRenderPanel::OnKeyDown)
	EVT_LEFT_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_RIGHT_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_MOTION(SimulationRenderPanel::OnMouseMotion)
	EVT_MOUSEWHEEL(SimulationRenderPanel::OnMouseWheel)
    EVT_PAINT(SimulationRenderPanel::OnPaint)
	EVT_SIZE(SimulationRenderPanel::OnSize)
wxEND_EVENT_TABLE()

SimulationRenderPanel::SimulationRenderPanel(wxWindow* parent, int* attribList)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : wxGLCanvas(parent, wxID_ANY, attribList,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE),
	m_shader(nullptr)
{
	m_simulationWindow = (SimulationWindow*) parent;

	// Force the OpenGL context to be created now.
	wxGetApp().GetGLContext(this);

	// Load resources.
	m_shader = LoadShader(
		"../../assets/shaders/generic_vs.glsl",
		"../../assets/shaders/generic_fs.glsl");
	m_shaderUnlitVertexColored = LoadShader(
		"../../assets/shaders/unlit_vertex_colored_vs.glsl",
		"../../assets/shaders/unlit_vertex_colored_fs.glsl");
	m_shaderUnlitColored = LoadShader(
		"../../assets/shaders/unlit_colored_vs.glsl",
		"../../assets/shaders/unlit_colored_fs.glsl");
	
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

	// Create agent mesh.
	{
		float h = 0.3f;
		Vector3f v1(0, h, -1);
		Vector3f v2(0.7f, h, 0.7f);
		Vector3f v3(-0.7f, h, 0.7f);
		Vector3f v4(0, 0, -1);
		Vector3f v5(0.7f, 0, 0.7f);
		Vector3f v6(-0.7f, 0, 0.7f);
		Vector3f n1 = Vector3f::UP;
		Vector3f n2 = Vector3f(1.7f, 0, 0.7f).Normalize();
		Vector3f n3 = Vector3f::BACK;
		Vector3f n4 = Vector3f(-1.7f, 0, 0.7f).Normalize();

		Vector4f color = Color::WHITE.ToVector4f();
		VertexPosNormCol vertices[] = {
			VertexPosNormCol(v1, n1, color), // Top
			VertexPosNormCol(v2, n1, color),
			VertexPosNormCol(v3, n1, color),
			VertexPosNormCol(v2, n2, color), // Right
			VertexPosNormCol(v1, n2, color),
			VertexPosNormCol(v4, n2, color),
			VertexPosNormCol(v5, n2, color),
			VertexPosNormCol(v3, n3, color), // Back
			VertexPosNormCol(v2, n3, color),
			VertexPosNormCol(v5, n3, color),
			VertexPosNormCol(v6, n3, color),
			VertexPosNormCol(v1, n4, color), // Left
			VertexPosNormCol(v3, n4, color),
			VertexPosNormCol(v6, n4, color),
			VertexPosNormCol(v4, n4, color),
		};
		unsigned int indices[] = {
			0, 1, 2,
			3, 4, 5, 3, 5, 6,
			7, 8, 9, 7, 9, 10,
			11, 12, 13, 11, 13, 14,
		};

		m_agentMesh = new Mesh();
		m_agentMesh->GetVertexData()->BufferVertices(15, vertices);
		m_agentMesh->GetIndexData()->BufferIndices(21, indices);

		m_agentMaterial = new Material();
		m_agentMaterial->SetColor(Color::GREEN);
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
}

SimulationRenderPanel::~SimulationRenderPanel()
{
	delete m_shader;
	
	delete m_agentMesh;
	delete m_meshSelectionCircle;

	delete m_worldMaterial;
	delete m_agentMaterial;
	delete m_materialSelectionCircle;
};

SimulationManager* SimulationRenderPanel::GetSimulationManager()
{
	return m_simulationWindow->GetSimulationManager();
}

Simulation* SimulationRenderPanel::GetSimulation()
{
	return m_simulationWindow->GetSimulationManager()->GetSimulation();
}


void SimulationRenderPanel::OnKeyDown(wxKeyEvent& e)
{
    switch (e.GetKeyCode())
    {
        case WXK_RIGHT:
            break;
        case WXK_LEFT:
            break;
        case WXK_DOWN:
            break;
        case WXK_UP:
            break;
        default:
		{
            e.Skip();
            return;
		}
    }
}

void SimulationRenderPanel::OnMouseDown(wxMouseEvent& e)
{
	if (e.LeftDown())
	{
		// Convert the screen coordinate to OpenGL clip space,
		// then convert that to a ray based on the camera's state.
		Vector2f screenCoord((float) e.GetX(), (float) e.GetY());
		const wxSize clientSize = GetClientSize();
		screenCoord /= Vector2f((float) clientSize.x, (float) clientSize.y);
		screenCoord = (screenCoord * 2.0f) - Vector2f::ONE;
		screenCoord.y = -screenCoord.y;
		Ray ray = GetSimulationManager()->GetCameraSystem()->GetRay(screenCoord);

		// Cast the ray onto the world sphere.
		float distance;
		if (GetSimulation()->GetWorld()->CastRay(ray, distance))
		{
			Vector3f point = ray.GetPoint(distance);
			
			// Check for selecting agents.
			AgentSystem* agentSystem = GetSimulation()->GetAgentSystem();
			GetSimulationManager()->SetSelectedAgent(nullptr);
			for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
			{
				Agent* agent = *it;
				if (agent->GetPosition().DistTo(point) < 0.1f)
				{
					GetSimulationManager()->SetSelectedAgent(agent);
					break;
				}
			}
		}

	}
	else if (e.RightDown())
	{

	}
}

void SimulationRenderPanel::OnMouseMotion(wxMouseEvent& e)
{
	static int mx = -1;
	static int my = -1;

	int mxPrev = mx;
	int myPrev = my;
	mx = e.GetX();
	my = e.GetY();
	
    const wxSize clientSize = GetClientSize();
	float aspectRatio = (float) clientSize.x / (float) clientSize.y;

	// Update camera controls.
	if (mxPrev != -1 && myPrev != -1 && e.RightIsDown())
	{
		float deltaX = aspectRatio * ((mx - mxPrev) / (float) clientSize.x);
		float deltaY = (my - myPrev) / (float) clientSize.y;

		ICamera* camera = GetSimulationManager()->GetActiveCamera();

		if (e.GetModifiers() & wxMOD_CONTROL)
			camera->AltRotate(deltaX, deltaY);
		else
			camera->Rotate(deltaX, deltaY);
	}
}

void SimulationRenderPanel::OnMouseWheel(wxMouseEvent& e)
{
	float mouseDelta = (float) e.GetWheelRotation() / 120.0f;

	// Adjust camera distance from surface.
	ICamera* camera = GetSimulationManager()->GetActiveCamera();
	camera->Zoom(mouseDelta);
}

void SimulationRenderPanel::OnSize(wxSizeEvent& e)
{
	const wxSize clientSize = GetClientSize();
	float aspectRatio = (float) clientSize.x / (float) clientSize.y;

	GetSimulationManager()->GetCameraSystem()->SetAspectRatio(aspectRatio);
}

void SimulationRenderPanel::OnPaint(wxPaintEvent& e)
{
	Simulation* simulation = GetSimulation();
	ICamera* camera = GetSimulationManager()->GetCameraSystem()->GetActiveCamera();
	Agent* selectedAgent = GetSimulationManager()->GetSelectedAgent();

	float worldRadius = simulation->GetWorld()->GetRadius();

    // This is required even though dc is not used otherwise.
    //wxPaintDC dc(this);

    const wxSize clientSize = GetClientSize();
    OpenGLContext& canvas = wxGetApp().GetGLContext(this);
    glViewport(0, 0, clientSize.x, clientSize.y);
	
	RenderParams renderParams;
	renderParams.EnableDepthTest(true);
	renderParams.EnableDepthBufferWrite(true);
	renderParams.EnableNearFarPlaneClipping(true);
	renderParams.EnableBlend(true);
	renderParams.EnableLineSmooth(false);
	renderParams.EnablePolygonSmooth(false);
	renderParams.EnableCullFace(true);
	renderParams.SetCullFace(CullFace::BACK);
	renderParams.SetFrontFace(FrontFace::CLOCKWISE);
	renderParams.SetClearBits(ClearBits::COLOR_BUFFER_BIT | ClearBits::DEPTH_BUFFER_BIT);
	renderParams.SetClearColor(Color::BLACK);
	renderParams.SetPolygonMode(PolygonMode::FILL);
	renderParams.SetDepthFunction(CompareFunction::LESS_EQUAL);
	renderParams.SetBlendFunction(BlendFunc::SOURCE_ALPHA, BlendFunc::ONE_MINUS_SOURCE_ALPHA);
	if (GetSimulationManager()->IsViewWireFrameMode())
		renderParams.SetPolygonMode(PolygonMode::LINE);
	else
		renderParams.SetPolygonMode(PolygonMode::FILL);

	m_renderer.SetRenderParams(renderParams);
	m_renderer.ApplyRenderSettings(true);
	m_renderer.SetCamera(camera);
	m_renderer.SetShader(m_shader);

	// Render the world.
	Transform3f transform;
	transform.SetIdentity();
	transform.SetScale(worldRadius);
	m_renderer.SetShader(m_shader);
	m_renderer.RenderMesh(simulation->GetWorld()->GetMesh(), m_worldMaterial, transform);
	
	// Render the agents.
	AgentSystem* agentSystem = simulation->GetAgentSystem();
	float agentRadius = 0.016f;
	float agentOffset = worldRadius - Math::Sqrt((worldRadius * worldRadius) - (agentRadius * agentRadius));
	for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	{
		Agent* agent = *it;
		// Create the agent transform and render it.
		transform.pos = agent->GetPosition();
		transform.pos.Normalize();
		transform.pos *= worldRadius - agentOffset;
		transform.rot = agent->GetOrientation();
		transform.SetScale(agentRadius);
		m_renderer.SetShader(m_shader);
		m_renderer.RenderMesh(m_agentMesh, m_agentMaterial, transform);

		// Draw the selection circle.
		if (selectedAgent == agent)
		{	
			transform.scale *= 1.2f;
			transform.pos.Normalize();
			transform.pos *= worldRadius + 0.001f;
			m_renderer.SetShader(m_shaderUnlitColored);
			m_renderer.RenderMesh(m_meshSelectionCircle, m_materialSelectionCircle, transform);
		}
	}
	
	// Render the X/Y/Z axis lines.
	transform.SetIdentity();
	transform.SetScale(worldRadius * 20.0f);
	m_renderer.SetShader(m_shaderUnlitVertexColored);
	m_renderer.RenderMesh(m_meshAxisLines, m_materialAxisLines, transform);

    // Swap the display buffers.
    SwapBuffers();
}
