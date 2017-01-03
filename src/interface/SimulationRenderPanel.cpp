#include "SimulationRenderPanel.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Simulation.h>


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
    EVT_TIMER(TICK_TIMER, SimulationRenderPanel::OnTickTimer)
    EVT_PAINT(SimulationRenderPanel::OnPaint)
wxEND_EVENT_TABLE()

SimulationRenderPanel::SimulationRenderPanel(Simulation* simulation, wxWindow* parent, int* attribList)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : wxGLCanvas(parent, wxID_ANY, attribList,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE),
	  m_scale(0.8f),
	  m_cameraRotation(Quaternion::IDENTITY),
	  m_simulation(simulation),
      m_tickTimer(this, TICK_TIMER)
{
	m_tickTimer.Start(17);

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	m_globeCamera.Initialize(Vector3f::ZERO, worldRadius, worldRadius * 2, 0.0f);

	m_camera = &m_globeCamera;
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
        case WXK_SPACE:
		{
			m_simulation->GetAgentSystem()->SpawnAgent();
            break;
		}
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
		// TODO: check for selecting agents.
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

	// Update camera controls.
	if (mxPrev != -1 && myPrev != -1 && e.RightIsDown())
	{
		float deltaX = mx - mxPrev;
		float deltaY = my - myPrev;

		if (e.GetModifiers() & wxMOD_CONTROL)
			m_globeCamera.RotateSurfaceCamera(deltaX, deltaY);
		else
			m_globeCamera.RotateAroundGlobe(deltaX, deltaY);
	}
}

void SimulationRenderPanel::OnMouseWheel(wxMouseEvent& e)
{
	float mouseDelta = (float) e.GetWheelRotation() / 120.0f;
	
	// Adjust camera distance from surface.
	m_globeCamera.Zoom(mouseDelta);
}

void SimulationRenderPanel::OnTickTimer(wxTimerEvent& e)
{
	// 1. Update simulation.
	m_simulation->Tick();

	// 2. Render the simulation, this will call OnPaint()
    Refresh(false);
}

void SimulationRenderPanel::OnPaint(wxPaintEvent& e)
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

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

	Matrix4f projection = Matrix4f::CreatePerspective(1.4f, aspectRatio, 0.01f, 100.0f);
	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	m_camera->SetProjection(Matrix4f::CreatePerspective(1.4f, aspectRatio, 0.01f, 100.0f));

	Matrix4f viewMatrix = Matrix4f::IDENTITY;
	viewMatrix *= Matrix4f::CreateTranslation(0.0f, 0.0f, -2.0f);
	viewMatrix *= Matrix4f::CreateRotation(m_cameraRotation);
	viewMatrix *= Matrix4f::CreateScale(m_scale);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_camera->GetViewProjection().GetTranspose().data());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(worldRadius, worldRadius, worldRadius);

	// Draw the world.
	std::vector<Vector3f>& vertices = m_simulation->GetWorld()->GetVertices();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.7f, 0.1f);
	glColor3f(0.05f, 0.05f, 0.05f);
	Vector3f normal;
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (i % 3 == 0)
		{
			Vector3f v1 = vertices[i];
			Vector3f v2 = vertices[i + 1];
			Vector3f v3 = vertices[i + 2];
			normal = (v3 - v2).Cross(v2 - v1);
			normal.Normalize();
		}

		glNormal3fv(normal.data());
		glColor3fv(((normal + Vector3f::ONE) * 0.5f).data());
		glVertex3fv(vertices[i].data());
	}
	glEnd();
	/*
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.7f, 0.1f);
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (i % 3 == 0)
		{
			Vector3f v1 = vertices[i];
			Vector3f v2 = vertices[i + 1];
			Vector3f v3 = vertices[i + 2];
			normal = (v3 - v2).Cross(v2 - v1);
			normal.Normalize();
		}

		glNormal3fv(normal.data());//vertices[i].Normalize().data());
		glColor3fv(((normal + Vector3f::ONE) * 0.5f).data());
		glVertex3fv((vertices[i] * 1.02).data());
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw the agents.
	AgentSystem* agentSystem = m_simulation->GetAgentSystem();
	for (auto it = agentSystem->agents_begin(); it != agentSystem->agents_end(); it++)
	{
		Agent* agent = *it;

		glBegin(GL_LINES);
			glColor3f(0.0f, 1.0f, 0.0f);
			Vector3f up = agent->GetPosition() + agent->GetOrientation().GetUp() * 0.02f;
			Vector3f forward = agent->GetPosition() + agent->GetOrientation().GetForward() * 0.02f;
			glVertex3fv(agent->GetPosition().data());
			glVertex3fv(up.data());
			glVertex3fv(agent->GetPosition().data());
			glVertex3fv(forward.data());
		glEnd();
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
	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINES);
		glColor3f(1,1,1);
		glVertex3fv(m_globeCamera.GetSurfacePosition().data());
		glVertex3fv((m_globeCamera.GetSurfacePosition() * 1.1f).data());
	glEnd();

    // Swap the buffers.
    SwapBuffers();
}
