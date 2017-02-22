#include "OctTreeRenderer.h"
#include "SimulationManager.h"
#include <graphics/OpenGLIncludes.h>
#include <math/Matrix3f.h>


//-----------------------------------------------------------------------------
// Bounds2D
//-----------------------------------------------------------------------------

bool Bounds2D::CircleContains(const Vector2f& point) const
{
	return (point.DistToSqr(center) < radius * radius);
}
	
bool Bounds2D::SquareContains(const Vector2f& point) const
{
	return (Math::Abs(point.x - center.x) < radius &&
			Math::Abs(point.y - center.y) < radius);
}
	
bool Bounds2D::SquareContainsSquare(const Bounds2D& bounds) const
{
	return (bounds.center.x - bounds.radius >= center.x - radius &&
			bounds.center.x + bounds.radius <= center.x + radius &&
			bounds.center.y - bounds.radius >= center.y - radius &&
			bounds.center.y + bounds.radius <= center.y + radius);
}
	
bool Bounds2D::SquareIntersectsSquare(const Bounds2D& bounds) const
{
	return (bounds.center.x + bounds.radius > center.x - radius &&
			center.x + radius > bounds.center.x - bounds.radius &&
			bounds.center.y + bounds.radius > center.y - radius &&
			center.y + radius > bounds.center.y - bounds.radius);
}


//-----------------------------------------------------------------------------
// SectorFace
//-----------------------------------------------------------------------------

Vector3f SectorFace::Get3DPoint(const Vector2f& pointOnFacePlane) const
{
	Vector3f point3d;
	point3d[axis] = posOnAxis;
	point3d[(axis + 1) % 3] = pointOnFacePlane.x;
	point3d[(axis + 2) % 3] = pointOnFacePlane.y;
	return point3d;
}


//-----------------------------------------------------------------------------
// ContourCurve
//-----------------------------------------------------------------------------

ContourCurve::ContourCurve() :
	fullCircle(false)
{
}

ContourCurve::ContourCurve(const Vector2f& startPoint,
							const Vector2f& endPoint) :
	startPoint(startPoint),
	endPoint(endPoint),
	fullCircle(false)
{
}

ContourCurve::ContourCurve(bool fullCircle) :
	fullCircle(fullCircle)
{
}



//-----------------------------------------------------------------------------
// OctTreeRenderer
//-----------------------------------------------------------------------------

OctTreeRenderer::OctTreeRenderer(SimulationManager* simulationManager,
								ResourceManager* resourceManager) :
	m_octTree(nullptr),
	m_renderer(nullptr),
	m_shaderUnlit(nullptr),
	m_resourceManager(resourceManager),
	m_simulationManager(simulationManager)
{
}

void OctTreeRenderer::Initialize()
{
	// Get resources.
	m_shaderUnlit = m_resourceManager->GetShader("unlit");
}

void OctTreeRenderer::OnNewSimulation(Simulation* simulation)
{
	m_worldSphere.position = Vector3f::ZERO;
	m_worldSphere.radius = simulation->GetWorld()->GetRadius();
}


//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------

void OctTreeRenderer::RenderOctTree(Renderer* renderer, OctTree* octTree)
{
	m_renderer = renderer;
	m_octTree = octTree;

	// Setup the renderer.
	//RenderParams renderParams = m_renderer->GetRenderParams();
	//renderParams.EnableCullFace(false);
	//m_renderer->SetRenderParams(renderParams);
	//m_renderer->ApplyRenderSettings(false);

	// Render the OctTree.

	// Render the wireframe.
	if (m_simulationManager->GetShowOctTreeWireFrame())
		RenderWireFrame(Color::YELLOW * 0.5f);

	// Render the surface contours.
	if (m_simulationManager->GetShowOctTree())
		RenderSurfaceContours(Color::YELLOW);
}

void OctTreeRenderer::RenderWireFrame(const Color& color)
{
	// Create a material to render with.
	Material material;
	material.SetColor(color);
	material.SetIsLit(false);
	
	// Render the wire frame of the oct-tree.
	m_renderer->SetShader(m_shaderUnlit);
	m_renderer->UpdateUniforms(&material, Matrix4f::IDENTITY);
	RenderSectorBoxes(m_octTree->GetRootNode(),
		m_octTree->GetBounds(), 0);
}

void OctTreeRenderer::RenderSurfaceContours(const Color& color)
{
	// Create a material to render with.
	Material material;
	material.SetColor(color);
	material.SetIsLit(false);
		
	// Render the intersection contours between the
	// octtree and the world sphere.
	m_renderer->SetShader(m_shaderUnlit);
	m_renderer->UpdateUniforms(&material,
		Matrix4f::CreateScale(1.005f));
	RenderContoursForSector(m_octTree->GetRootNode(),
		m_octTree->GetBounds(), 0);
	glLineWidth(1.0f);
}



//-----------------------------------------------------------------------------
// Wireframe rendering
//-----------------------------------------------------------------------------

void OctTreeRenderer::RenderSectorBoxes(OctTreeNode* node,
										const AABB& sectorBounds,
										int depth)
{
	DrawSectorWireFrame(sectorBounds);

	// Recursively render the child sectors.
	for (int sectorIndex = 0; sectorIndex < 8; sectorIndex++)
	{
		AABB childBounds = sectorBounds;
		OctTreeNode* childNode = m_octTree->TraverseIntoSector(
			node, sectorIndex, childBounds);
		if (childNode != nullptr)
			RenderSectorBoxes(childNode, childBounds, depth + 1);
	}
}

// Render a wire-frame box for a sector.
void OctTreeRenderer::DrawSectorWireFrame(const AABB& bounds)
{
	glBegin(GL_LINES);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
	
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z);
		
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z);
	glEnd();
}



//-----------------------------------------------------------------------------
// Contour rendering
//-----------------------------------------------------------------------------

// Render the contour lines created from the intersection of
// this node's cube and the sphere.
void OctTreeRenderer::RenderContoursForSector(	OctTreeNode* node,
												const AABB& sectorBounds,
												int depth)
{
	Vector3f boundsCenter = sectorBounds.GetCenter();
	float boundsRadius = (sectorBounds.maxs.x - sectorBounds.mins.x) * 0.5f;
	
	// Quickly check if the sector bounds doesn't intersect the world sphere.
	float sectorMaxRadius = sectorBounds.maxs.DistTo(sectorBounds.mins) * 0.5f;
	float radiusSum = m_worldSphere.radius + boundsRadius;
	if (boundsCenter.DistToSqr(m_worldSphere.position) > radiusSum * radiusSum)
		return;

	// Create a list of square faces for the sector cube.
	// Faces are ordered by: axis (X, Y, Z), then side (+, -).
	SectorFace sectorFaces[6];
	for (int i = 0; i < 6; i++)
	{
		SectorFace& face = sectorFaces[i];
		face.axis = i / 2;
		face.posOnAxis = (i % 2 == 0 ?
			sectorBounds.maxs[face.axis] : sectorBounds.mins[face.axis]);
		face.rectOnFace.center.x = boundsCenter[(face.axis + 1) % 3];
		face.rectOnFace.center.y = boundsCenter[(face.axis + 2) % 3];
		face.rectOnFace.radius = boundsRadius;
	}
					
	// Calculate and render the contours for each face.
	unsigned int totalContours = 0;
	glLineWidth(1.0f + (float) (m_octTree->GetMaxDepth() - depth));
	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
		totalContours += RenderContoursForFace(sectorFaces[faceIndex]);

	// Draw the sector wire frame if it intersects the world sphere.
	if (totalContours > 0 && m_simulationManager->GetShowOctTreeWireFrame())
	{
		glLineWidth(1.0f);
		DrawSectorWireFrame(sectorBounds);
	}

	// Recursively render the child sectors.
	for (int sectorIndex = 0; sectorIndex < 8; sectorIndex++)
	{
		AABB childBounds = sectorBounds;
		OctTreeNode* childNode = m_octTree->TraverseIntoSector(
			node, sectorIndex, childBounds);
		if (childNode != nullptr)
			RenderContoursForSector(childNode, childBounds, depth + 1);
	}
}

unsigned int OctTreeRenderer::RenderContoursForFace(const SectorFace& face)
{
	// Escape early if this face is not touching the world sphere.
	if (Math::Abs(face.posOnAxis) > m_worldSphere.radius)
		return 0;

	// Calculate the circle cross-section of the sphere on this face's plane;
	Bounds2D circle;
	circle.radius = Math::Sqrt((m_worldSphere.radius *
		m_worldSphere.radius) - (face.posOnAxis * face.posOnAxis));
	circle.center = Vector2f::ZERO;

	// Generate a list of contours between the square and circle.
	ContourCurve contours[6];
	unsigned int numContours = GenerateContours(
		circle, face.rectOnFace, contours);

	// Render each contour curve.
	for (unsigned int i = 0; i < numContours; i++)
		DrawContour(face, circle, contours[i]);

	return numContours;
}

// Draw a single contour curve.
void OctTreeRenderer::DrawContour(	const SectorFace& face,
									const Bounds2D& circle,
									const ContourCurve& contour)
{
	if (contour.fullCircle)
		DrawCircleContour(face, circle);
	else
		DrawArcContour(face, circle, contour.endPoint, contour.startPoint);
}

// Draw a contour that is a full circle outline.
void OctTreeRenderer::DrawCircleContour(const SectorFace& face,
										const Bounds2D& circle)
{
	Vector2f v2;

	// Render a circle outline.
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 40; i++)
	{
		float angle = (i / 40.0f) * Math::TWO_PI;
		v2.x = circle.center.x + cos(angle) * circle.radius;
		v2.y = circle.center.y - sin(angle) * circle.radius;
		glVertex3fv(face.Get3DPoint(v2).data());
	}
	glEnd();
}

// Draw a contour that is an arc.
void OctTreeRenderer::DrawArcContour(const SectorFace& face,
									const Bounds2D& circle,
									Vector2f a, Vector2f b)
{
	Vector2f aDir = (a - circle.center); aDir.Normalize();
	Vector2f bDir = (b - circle.center); bDir.Normalize();
	Vector2f normal = (b - a);
	normal = Vector2f(-normal.y, normal.x);

	// Determine the angle between the two points.
	float angleBetween = Math::ACos(aDir.Dot(bDir));
	if ((circle.center - a).Dot(normal) > 0.0f)
		angleBetween = Math::TWO_PI - angleBetween;
	int numSteps = 1 + (int) ((angleBetween * 40) / Math::TWO_PI);
	float angleStep = angleBetween / (float) numSteps;
	Matrix3f rotation = Matrix3f::CreateRotation(circle.center, angleStep);
	
	// Draw the arc vertices.
	glBegin(GL_LINE_STRIP);
	glVertex3fv(face.Get3DPoint(a).data());
	Vector2f v2 = a;
	for (int i = 0; i < numSteps; i++)
	{
		v2 = rotation.TransformVector(v2);
		glVertex3fv(face.Get3DPoint(v2).data());
	}
	glVertex3fv(face.Get3DPoint(b).data());
	glEnd();
}
	

//-----------------------------------------------------------------------------
// Contour calculation
//-----------------------------------------------------------------------------

unsigned int OctTreeRenderer::GenerateContours(	const Bounds2D& circle,
												const Bounds2D& square,
												ContourCurve* contours)
{
	// Check if the square fully contains the circle.
	if (square.SquareContainsSquare(circle))
	{
		contours[0].fullCircle = true;
		return 1;
	}
	
	unsigned int numContours = 0;

	// Quickly check if the circle is not touching the square.
	if (!square.SquareIntersectsSquare(circle))
		return 0;

	Vector2f corners[4];
	bool circleContainsCorner[4];
	bool squareContainsCorner[4];

	// Compute square corner locations.
	corners[0] = Vector2f(	square.center.x - square.radius,
							square.center.y - square.radius);
	corners[1] = Vector2f(	square.center.x - square.radius,
							square.center.y + square.radius);
	corners[2] = Vector2f(	square.center.x + square.radius,
							square.center.y + square.radius);
	corners[3] = Vector2f(	square.center.x + square.radius,
							square.center.y - square.radius);

	for (int i = 0; i < 4; i++)
	{
		circleContainsCorner[i] = circle.CircleContains(corners[i]);
		squareContainsCorner[i] = circle.SquareContains(corners[i]);
	}
	bool circleBoundsContainsSquare = circle.SquareContainsSquare(square);
	
	// Loop over corners/edges.
	for (int i = 0; i < 4; i++)
	{
		int currCornerIndex = i;
		int prevCornerIndex = (i + 3) % 4;
		int nextCornerIndex = (i + 1) % 4;
		Vector2f corner = corners[i];
		Vector2f normalX = Vector2f(corner.x - square.center.x, 0.0f);
		Vector2f normalY = Vector2f(0.0f, corner.y - square.center.y);
		bool onInside = ((circle.center - corner).Dot(normalX) < 0.0f &&
						(circle.center - corner).Dot(normalY) < 0.0f);

		// Corner case.
		if (squareContainsCorner[currCornerIndex] &&
			!circleContainsCorner[currCornerIndex] &&
			onInside)
		{
			contours[numContours++] = ContourCurve(
				ComputeIntersectionPoint(circle, i, 0, corners),
				ComputeIntersectionPoint(circle, i - 1, 1, corners));
		}

		// Opposite edges case.
		if (squareContainsCorner[currCornerIndex] &&
			squareContainsCorner[nextCornerIndex] &&
			!squareContainsCorner[prevCornerIndex] &&
			(onInside || (circleContainsCorner[currCornerIndex] &&
				circleContainsCorner[nextCornerIndex])))
		{
			contours[numContours++] = ContourCurve(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i + 1, 1, corners));
		}

		// Adjacent edges case.
		else if (squareContainsCorner[currCornerIndex] &&
			((!squareContainsCorner[prevCornerIndex] || !onInside) &&
				!circleContainsCorner[prevCornerIndex]) &&
			((!squareContainsCorner[nextCornerIndex] || !onInside) &&
				!circleContainsCorner[nextCornerIndex]) &&
			!circleBoundsContainsSquare &&
			(onInside || circleContainsCorner[currCornerIndex]))
		{
			contours[numContours++] = ContourCurve(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i, 1, corners));
		}
	}
	
	if (numContours == 0)
	{
		// Loop over edges.
		for (int i = 0; i < 4; i++)
		{
			int currCornerIndex = i;
			int nextCornerIndex = (i + 1) % 4;
			Vector2f corner = corners[i];
			Vector2f nextCorner = corners[(i + 1) % 4];
		
			// Single edge case.
			Vector2f tangent = Vector2f::Normalize(nextCorner - corner);
			Vector2f normal(tangent.y, -tangent.x);
			Vector2f edgeCenter = (corner + nextCorner) * 0.5f;
			int normalAxis = i % 2;

			if (!circleContainsCorner[currCornerIndex] &&
				!circleContainsCorner[nextCornerIndex] &&
				Math::Abs((circle.center - edgeCenter).Dot(normal)) < circle.radius &&
				Math::Abs((circle.center - edgeCenter).Dot(tangent)) < square.radius)
			{
				contours[numContours++] = ContourCurve(
					ComputeIntersectionPoint(circle, i, 0, corners),
					ComputeIntersectionPoint(circle, i, 1, corners));
				break;
			}
		}
	}

	return numContours;
}

Vector2f OctTreeRenderer::ComputeIntersectionPoint(const Bounds2D& circle,
		int edgeIndex, int pointIndex, Vector2f* corners)
{
	if (edgeIndex < 0)
		edgeIndex += 4;
	if (edgeIndex >= 4)
		edgeIndex -= 4;

	Vector2f point;
	int normalAxis = edgeIndex % 2;
	int tangentAxis = 1 - normalAxis;
	float aa = corners[edgeIndex][normalAxis] - circle.center[normalAxis];
	float sqrSol = (circle.radius * circle.radius) - (aa * aa);

	if (sqrSol > 0.0f)
	{
		point[normalAxis] = corners[edgeIndex][normalAxis];

		point[tangentAxis] = Math::Sqrt(sqrSol);
		if (edgeIndex < 2)
			point[tangentAxis] = -point[tangentAxis];
		if (pointIndex == 1)
			point[tangentAxis] = -point[tangentAxis];
		point[tangentAxis] += circle.center[tangentAxis];
	}

	return point;
}

