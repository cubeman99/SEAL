#include "SimulationRenderPanel.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    SpinTimer = wxID_HIGHEST + 1,
	RenderTimer,
};

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

static void CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            wxLogError(wxT("OpenGL error state couldn't be reset."));
            return;
        }

        errLast = err;

        wxLogError(wxT("OpenGL error %d"), err);
    }
}

// function to draw the texture for cube faces
static wxImage DrawDice(int size, unsigned num)
{
    wxASSERT_MSG( num >= 1 && num <= 6, wxT("invalid dice index") );

    const int dot = size/16;        // radius of a single dot
    const int gap = 5*size/32;      // gap between dots

    wxBitmap bmp(size, size);
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBrush(*wxBLACK_BRUSH);

    // the upper left and lower right points
    if ( num != 1 )
    {
        dc.DrawCircle(gap + dot, gap + dot, dot);
        dc.DrawCircle(size - gap - dot, size - gap - dot, dot);
    }

    // draw the central point for odd dices
    if ( num % 2 )
    {
        dc.DrawCircle(size/2, size/2, dot);
    }

    // the upper right and lower left points
    if ( num > 3 )
    {
        dc.DrawCircle(size - gap - dot, gap + dot, dot);
        dc.DrawCircle(gap + dot, size - gap - dot, dot);
    }

    // finally those 2 are only for the last dice
    if ( num == 6 )
    {
        dc.DrawCircle(gap + dot, size/2, dot);
        dc.DrawCircle(size - gap - dot, size/2, dot);
    }

    dc.SelectObject(wxNullBitmap);

    return bmp.ConvertToImage();
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// SimulationRenderPanel
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SimulationRenderPanel, wxGLCanvas)
    EVT_PAINT(SimulationRenderPanel::OnPaint)
    EVT_KEY_DOWN(SimulationRenderPanel::OnKeyDown)
	EVT_LEFT_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_RIGHT_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(SimulationRenderPanel::OnMouseDown)
	EVT_MOTION(SimulationRenderPanel::OnMouseMotion)
	EVT_MOUSEWHEEL(SimulationRenderPanel::OnMouseWheel)
    EVT_TIMER(SpinTimer, SimulationRenderPanel::OnSpinTimer)
wxEND_EVENT_TABLE()

SimulationRenderPanel::SimulationRenderPanel(wxWindow *parent, int *attribList)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : wxGLCanvas(parent, wxID_ANY, attribList,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE),
      m_xangle(30.0),
      m_yangle(30.0),
	  m_scale(0.4f),
	  m_cameraRotation(Quaternion::IDENTITY),
      m_spinTimer(this, SpinTimer)
{
    if ( attribList )
    {
        int i = 0;
        while ( attribList[i] != 0 )
        {
            //if ( attribList[i] == WX_GL_STEREO )
                //m_useStereo = true;
            ++i;
        }
    }
     
	m_spinTimer.Start(17);
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

	Matrix4f projection = Matrix4f::CreatePerspective(1.4f, aspectRatio, 0.01f, 100.0f);

    glBindTexture(GL_TEXTURE_2D, 0);
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glTranslatef(0.0f, 0.0f, -2.0f);
	Matrix4f viewMatrix = Matrix4f::IDENTITY;
	viewMatrix *= Matrix4f::CreateTranslation(0.0f, 0.0f, -2.0f);
	viewMatrix *= Matrix4f::CreateRotation(m_cameraRotation);
	viewMatrix *= Matrix4f::CreateScale(m_scale);
	glLoadMatrixf(viewMatrix.GetTranspose().data());
    //glRotatef(m_xangle, 1.0f, 0.0f, 0.0f);
    //glRotatef(m_yangle, 0.0f, 1.0f, 0.0f);
	//glScalef(m_scale, m_scale, m_scale);
    glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection.GetTranspose().data());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	std::vector<Vector3f>& vertices = m_world.GetVertices();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.7f, 0.1f);
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

		glNormal3fv(normal.data());//vertices[i].Normalize().data());
		glColor3fv(((normal + Vector3f::ONE) * 0.5f).data());
		glVertex3fv(vertices[i].data());
	}
	glEnd();

	for (unsigned int i = 0; i < m_agents.size(); i++)
	{
		Agent& agent = m_agents[i];

		Vector3f posPrev = agent.position;
		
		agent.position += agent.orientation.GetForward() * 0.006f;
		agent.position.Normalize();
		
		float angle = Math::ACos(agent.position.Dot(posPrev));
		agent.orientation.Rotate(agent.orientation.GetRight(), angle);
				
		glBegin(GL_LINES);
			glColor3f(0.0f, 1.0f, 0.0f);
			Vector3f up = agent.position + agent.orientation.GetUp() * 0.02f;
			Vector3f forward = agent.position + agent.orientation.GetForward() * 0.02f;
			glVertex3fv(agent.position.data());
			glVertex3fv(up.data());
			glVertex3fv(agent.position.data());
			glVertex3fv(forward.data());
		glEnd();
	}
	
	
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


    // Render the graphics and swap the buffers.
    //canvas.DrawRotatedCube(m_xangle, m_yangle);
    SwapBuffers();
}

void SimulationRenderPanel::Spin(float xSpin, float ySpin)
{
    m_xangle += xSpin;
    m_yangle += ySpin;

    Refresh(false);
}

void SimulationRenderPanel::OnKeyDown(wxKeyEvent& e)
{
    switch (e.GetKeyCode())
    {
        //case WXK_RIGHT:
        //    break;
        //case WXK_LEFT:
        //    break;
        //case WXK_DOWN:
        //    break;
        //case WXK_UP:
        //    break;
        case WXK_SPACE:
		{
			Agent agent;
			agent.position.x = Random::NextFloat(-1, 1);
			agent.position.y = Random::NextFloat(-1, 1);
			agent.position.z = Random::NextFloat(-1, 1);
			agent.position.Normalize();
			
			Vector3f axis = Vector3f::Normalize(agent.position.Cross(Vector3f::UP));
			float angle = Math::ACos(agent.position.Dot(Vector3f::UP));
			agent.orientation = Quaternion(axis, angle);
			agent.orientation.Rotate(agent.orientation.GetUp(), Random::NextFloat() * Math::TWO_PI);

			m_agents.push_back(agent);
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

	if (mxPrev != -1 && myPrev != -1 && e.RightIsDown())
	{
		m_yangle += (mx - mxPrev);
		m_xangle += (my - myPrev);

		m_cameraRotation.Rotate(Vector3f::DOWN, 0.006f * (mx - mxPrev));
		m_cameraRotation.Rotate(Vector3f::LEFT, 0.006f * (my - myPrev));
	}
	
    //Spin(1, 0);
	//m_xangle += 1;
    //Refresh(false);
}

void SimulationRenderPanel::OnMouseWheel(wxMouseEvent& e)
{
	float mouseDelta = (float) e.GetWheelRotation();
	mouseDelta /= 120.0f;
	m_scale *= Math::Pow(1.1f, (float) mouseDelta);
}

void SimulationRenderPanel::OnSpinTimer(wxTimerEvent& e)
{
    Spin(0.0, 0.5);
}

wxString glGetwxString(GLenum name)
{
    const GLubyte *v = glGetString(name);
    if ( v == 0 )
    {
        // The error is not important. It is GL_INVALID_ENUM.
        // We just want to clear the error stack.
        glGetError();

        return wxString();
    }

    return wxString((const char*)v);
}

