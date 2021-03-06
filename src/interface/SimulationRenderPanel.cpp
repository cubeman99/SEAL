#include "SimulationRenderPanel.h"
#include "OpenGLContext.h"
#include "MainApplication.h"
#include <math/Matrix4f.h>
#include <math/MathLib.h>
#include <utilities/Random.h>
#include <simulation/Agent.h>
#include <simulation/Simulation.h>
#include "SimulationWindow.h"

// ----------------------------------------------------------------------------
// Event Table
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


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

SimulationRenderPanel::SimulationRenderPanel(wxWindow* parent, SimulationWindow* simulationWindow) :
    wxGLCanvas(parent, wxID_ANY, nullptr,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE)
{
	m_simulationWindow = simulationWindow;

	// Force the OpenGL context to be created now.
	wxGetApp().GetGLContext(this);
}

SimulationRenderPanel::~SimulationRenderPanel()
{
	// The resource manager will automatically delete our resources.
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------

SimulationManager* SimulationRenderPanel::GetSimulationManager()
{
	return m_simulationWindow->GetSimulationManager();
}

Simulation* SimulationRenderPanel::GetSimulation()
{
	return m_simulationWindow->GetSimulationManager()->GetSimulation();
}


//-----------------------------------------------------------------------------
// UI Events
//-----------------------------------------------------------------------------

void SimulationRenderPanel::OnKeyDown(wxKeyEvent& e)
{
	//GetParent()->GetEventHandler()->ProcessEvent(e);
}

void SimulationRenderPanel::OnMouseDown(wxMouseEvent& e)
{
	// Left Mouse Button: Select agents.
	if (e.LeftDown())
	{
		// Convert the screen coordinate to OpenGL clip space,
		// then convert that to a ray based on the camera's view & projection.
		Vector2f screenCoord((float) e.GetX(), (float) e.GetY());
		const wxSize clientSize = GetClientSize();
		screenCoord /= Vector2f((float) clientSize.x, (float) clientSize.y);
		screenCoord = (screenCoord * 2.0f) - Vector2f::ONE;
		screenCoord.y = -screenCoord.y;
		Ray ray = GetSimulationManager()->GetCameraSystem()->GetRay(screenCoord);

		// Cast rays onto the bounding spheres of all simulation objects.
		// The closest, successful raycast will be the selected object.
		ObjectManager* objectManager = GetSimulation()->GetObjectManager();
		GetSimulationManager()->SetSelectedAgent(nullptr);
		float distance;
		float closestDistance;
		
		// Raycast onto the world sphere first to get the starting
		// closest-distance (so we don't select objects on the other
		// side of the world).
		GetSimulation()->GetWorld()->CastRay(ray, closestDistance);

		// Then, raycast onto all simulation objects.
		for (auto it = objectManager->agents_begin();
			it != objectManager->agents_end(); ++it)
		{
			Agent* agent = *it;
			if (agent->GetBoundingSphere().CastRay(ray, distance) &&
				distance < closestDistance)
			{
				closestDistance = distance;
				GetSimulationManager()->SetSelectedAgent(agent);
			}
		}

	}
	else if (e.RightDown())
	{
		// (right mouse button is for camera controls)
	}
	
	SetFocus();
	//GetParent()->GetEventHandler()->ProcessEvent(e);
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

		// Perform different type of rotation whether the Ctrl key is down.
		if (e.GetModifiers() & wxMOD_CONTROL)
			camera->AltRotate(-deltaX, -deltaY);
		else
			camera->Rotate(-deltaX, -deltaY);
	}
}

void SimulationRenderPanel::OnMouseWheel(wxMouseEvent& e)
{
	float mouseDelta = (float) e.GetWheelRotation() / 120.0f;

	// Mouse Wheel: Adjust camera distance from surface (zoom in/out).
	ICamera* camera = GetSimulationManager()->GetActiveCamera();
	camera->Zoom(mouseDelta);
}

void SimulationRenderPanel::OnSize(wxSizeEvent& e)
{
	// On resize window: update camera aspect ratio.
	const wxSize clientSize = GetClientSize();
	float aspectRatio = (float) clientSize.x / (float) clientSize.y;
	GetSimulationManager()->GetCameraSystem()->SetAspectRatio(aspectRatio);
}

void SimulationRenderPanel::OnPaint(wxPaintEvent& e)
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);
	
	wxGetApp().GetGLContext(this);

	// Get the view port size.
    const wxSize clientSize = GetClientSize();
	Vector2f viewPortSize;
	viewPortSize.x = (float) clientSize.x;
	viewPortSize.y = (float) clientSize.y;
	
	// Render the simulation.
	SimulationRenderer* simulationRenderer =
		GetSimulationManager()->GetSimulationRenderer();
	simulationRenderer->Render(viewPortSize);
    
	// Swap the display buffers.
    SwapBuffers();
}

