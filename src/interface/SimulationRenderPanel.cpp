#include "SimulationRenderPanel.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Simulation.h>
#include "SimulationWindow.h"


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
	TICK_TIMER = wxID_HIGHEST + 1,
};

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
                 wxFULL_REPAINT_ON_RESIZE)
{
	m_simulationWindow = (SimulationWindow*) parent;
}

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

			// DEBUG: spawn a test ray to visualize the ray cast.
			ray.origin = point;
			ray.direction = -ray.direction;
			m_testRays.push_back(ray);
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
	Simulation* m_simulation = GetSimulation();
	ICamera* camera = GetSimulationManager()->GetCameraSystem()->GetActiveCamera();
	Agent* m_selectedAgent = GetSimulationManager()->GetSelectedAgent();

    // This is required even though dc is not used otherwise.
    //wxPaintDC dc(this);

    // Set the OpenGL viewport according to the client size of this canvas.
    // This is done here rather than in a wxSizeEvent handler because our
    // OpenGL rendering context (and thus viewport setting) is used with
    // multiple canvases: If we updated the viewport in the wxSizeEvent
    // handler, changing the size of one canvas causes a viewport setting that
    // is wrong when next another canvas is repainted.
    const wxSize clientSize = GetClientSize();

    OpenGLContext& canvas = wxGetApp().GetGLContext(this);
    glViewport(0, 0, clientSize.x, clientSize.y);
	
	float aspectRatio = (float) clientSize.x / (float) clientSize.y;

    /*glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);*/
	
    glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_COLOR_MATERIAL);
	
    glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
    glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(camera->GetViewProjection().GetTranspose().data());

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(worldRadius, worldRadius, worldRadius);

	// Draw the world.
	std::vector<Vector3f>& vertices = m_simulation->GetWorld()->GetVertices();
	glBegin(GL_TRIANGLES);
	glColor3f(0.05f, 0.05f, 0.05f);
	Vector3f normal;
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		/*if (i % 3 == 0)
		{
			Vector3f v1 = vertices[i];
			Vector3f v2 = vertices[i + 1];
			Vector3f v3 = vertices[i + 2];
			normal = (v3 - v2).Cross(v2 - v1);
			normal.Normalize();
		}*/
		normal = vertices[i];
		normal.Normalize();

		glNormal3fv(normal.data());
		glColor3fv(((normal + Vector3f::ONE) * 0.5f).data());
		glVertex3fv(vertices[i].data());
	}
	glEnd();
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glBegin(GL_TRIANGLES);
	//glColor3f(0.1f, 0.7f, 0.1f);
	//for (unsigned int i = 0; i < vertices.size(); i++)
	//{
	//	if (i % 3 == 0)
	//	{
	//		Vector3f v1 = vertices[i];
	//		Vector3f v2 = vertices[i + 1];
	//		Vector3f v3 = vertices[i + 2];
	//		normal = (v3 - v2).Cross(v2 - v1);
	//		normal.Normalize();
	//	}

	//	glNormal3fv(normal.data());//vertices[i].Normalize().data());
	//	glColor3fv(((normal + Vector3f::ONE) * 0.5f).data());
	//	glVertex3fv((vertices[i] * 1.0001).data());
	//}
	//glEnd();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw the agents.
	AgentSystem* agentSystem = m_simulation->GetAgentSystem();
	float r = 0.016f;
	float h = 0.0005f;
	Vector3f v1(0, h, -r);
	Vector3f v2(r * 0.7f, h, r * 0.8f);
	Vector3f v3(-r * 0.7f, h, r * 0.8f);
	for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	{
		Agent* agent = *it;

		Matrix4f modelMatrix = Matrix4f::CreateTranslation(agent->GetPosition()) *
			Matrix4f::CreateRotation(agent->GetOrientation());
				
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(modelMatrix.GetTranspose().data());
		
		// Draw agent.
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(v1.data());
			glVertex3fv(v2.data());
			glVertex3fv(v3.data());
		glEnd();
		glBegin(GL_TRIANGLES);
			glColor3f(0.5f, 1.0f, 0.5f);
			glVertex3fv(v1.data());
			glVertex3fv(v2.data());
			glVertex3fv(v3.data());
		glEnd();

		// Draw selection circle.
		if (m_selectedAgent == agent)
		{
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0f, 1.0f, 0.0f);
			for (int i = 0; i < 20; i++)
			{
				float a = (i / 20.0f) * Math::TWO_PI;
				glVertex3f(cos(a) * r * 1.2f, h, sin(a) * r * 1.2f);
			}
			glEnd();
		}
	}
	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(worldRadius, worldRadius, worldRadius);
	
	// Draw x/y/z axis lines.
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(2.0f, 0.0f, 0.0f);
		glVertex3f(-2.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(0.0f, -2.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 2.0f);
		glVertex3f(0.0f, 0.0f, -2.0f);
	glEnd();
	
	// DEBUG: Render some test rays.
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (unsigned int i = 0; i < m_testRays.size(); i++)
	{
		glVertex3fv(m_testRays[i].origin.data());
		glVertex3fv(m_testRays[i].GetPoint(0.5f).data());
	}
	glEnd();

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
    // Swap the buffers.
    SwapBuffers();
}
