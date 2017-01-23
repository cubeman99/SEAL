#include "OctTreeRenderer.h"
#include "SimulationManager.h"
#include <graphics/OpenGLIncludes.h>
#include <math/Matrix3f.h>


//-----------------------------------------------------------------------------
// CircleContour - Contour helper structure
//-----------------------------------------------------------------------------

CircleContour::CircleContour() :
	fullCircle(false)
{
}

CircleContour::CircleContour(const Vector2f& startPoint,
							const Vector2f& endPoint) :
	startPoint(startPoint),
	endPoint(endPoint),
	fullCircle(false)
{
}

CircleContour::CircleContour(bool fullCircle) :
	fullCircle(fullCircle)
{
}

//-----------------------------------------------------------------------------
// Bounds2D - Contour helper structure
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



//-----------------------------------------------------------------------------
// OctTreeRenderer
//-----------------------------------------------------------------------------

OctTreeRenderer::OctTreeRenderer() :
	m_octTree(nullptr),
	m_renderer(nullptr),
	m_shaderUnlit(nullptr),
	m_simulationManager(nullptr)
{
}

void OctTreeRenderer::Initialize(ResourceManager* resourceManager,
								SimulationManager* simulationManager)
{
	m_simulationManager = simulationManager;

	m_shaderUnlit = resourceManager->GetShader("unlit");

	m_worldSphere.position = Vector3f::ZERO;
	m_worldSphere.radius = m_simulationManager->
		GetSimulation()->GetWorld()->GetRadius();
}


//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------

void OctTreeRenderer::RenderOctTree(Renderer* renderer, OctTree* octTree)
{
	m_renderer = renderer;
	m_octTree = octTree;

	// Setup the renderer.
	RenderParams renderParams = m_renderer->GetRenderParams();
	renderParams.EnableCullFace(false);
	m_renderer->SetRenderParams(renderParams);
	m_renderer->ApplyRenderSettings(false);

	// Render the OctTree.
	RenderWireFrame();
	RenderSurfaceContours();
}

void OctTreeRenderer::RenderWireFrame()
{
	// Create a material to render with.
	Material material;
	material.SetColor(Color::YELLOW * 0.5f);
	material.SetIsLit(false);
	
	// Render the wire frame of the oct-tree.
	m_renderer->SetShader(m_shaderUnlit);
	m_renderer->UpdateUniforms(&material, Matrix4f::IDENTITY);
	RenderSectorBoxes(m_octTree->GetRootNode(),
		m_octTree->GetBounds(), 0);
}

void OctTreeRenderer::RenderSurfaceContours()
{
	// Create a material to render with.
	Material material;
	material.SetColor(Color::YELLOW);
	material.SetIsLit(false);
		
	// Render the intersection contours between the
	// octtree and the world sphere.
	m_renderer->SetShader(m_shaderUnlit);
	m_renderer->UpdateUniforms(&material,
		Matrix4f::CreateScale(1.005f));
	RenderSectorContours(m_octTree->GetRootNode(),
		m_octTree->GetBounds(), 0);
	glLineWidth(1.0f);
}



//-----------------------------------------------------------------------------
// Private render methods
//-----------------------------------------------------------------------------

void OctTreeRenderer::RenderSectorBoxes(OctTreeNode* node,
										const AABB& sectorBounds,
										int depth)
{
	// Render this sector box.
	DrawSector(sectorBounds);

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

void OctTreeRenderer::RenderSectorContours(	OctTreeNode* node,
											const AABB& sectorBounds,
											int depth)
{
	// Render the contour lines created from the intersection of
	// this node's cube and the sphere.

	Vector3f boundsCenter = sectorBounds.GetCenter();
	float boundsRadius = (sectorBounds.maxs.x - sectorBounds.mins.x) * 0.5f;
	
	// Quickly check if the sector bounds doesn't intersect the world sphere.
	float sectorMaxRadius = sectorBounds.maxs.DistTo(sectorBounds.mins) * 0.5f;
	float radiusSum = m_worldSphere.radius + boundsRadius;
	if (boundsCenter.DistToSqr(m_worldSphere.position) > radiusSum * radiusSum)
		return;

	// Create a list of 2D faces for their respective axes.
	// Ordered by: axis (X, Y, Z), then side (+, -).
	Bounds2D faces[6];
	faces[0].center = boundsCenter.GetYZ(); // right	(X+)
	faces[1].center = boundsCenter.GetYZ(); // left		(X-)
	faces[2].center = boundsCenter.GetZX(); // top		(Y+)
	faces[3].center = boundsCenter.GetZX(); // bottom	(Y-)
	faces[4].center = boundsCenter.GetXY(); // back		(Z+)
	faces[5].center = boundsCenter.GetXY(); // front	(Z-)
		
	CircleContour contours[6];
	int numContours = 0;
	bool hasContours = false;
					
	glLineWidth(1.0f + (float) (m_octTree->GetMaxDepth() - depth));

	// Perform contour generation for each 2D face.
	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
	{
		faces[faceIndex].radius = boundsRadius;
		int axis = faceIndex / 2;
		int xAxis = (axis + 1) % 3;
		int yAxis = (axis + 2) % 3;
		float posOnAxis = (faceIndex % 2 == 0 ?
			sectorBounds.maxs[axis] : sectorBounds.mins[axis]);

		if (Math::Abs(posOnAxis) < m_worldSphere.radius)
		{
			// Generate a list of contours.
			Bounds2D circle;
			circle.radius = Math::Sqrt((m_worldSphere.radius *
				m_worldSphere.radius) - (posOnAxis * posOnAxis));
			circle.center = Vector2f::ZERO;
			PerformContour(circle, faces[faceIndex], contours, numContours);

			if (numContours > 0)
				hasContours = true;

			// Render the contours.
			for (int i = 0; i < numContours; i++)
			{
				Vector3f v;
				v[axis] = posOnAxis;

				// Render either a circle or arc contour.
				if (contours[i].fullCircle)
					DrawCircle(circle, axis, posOnAxis);
				else
					DrawArc(contours[i].endPoint, contours[i].startPoint, circle, axis, posOnAxis);
			}
		}
	}

	// Draw the sector wire frame if it intersects the world sphere.
	if (hasContours)
	{
		glLineWidth(1.0f);
		DrawSector(sectorBounds);
	}

	// Recursively render the child sectors.
	for (int sectorIndex = 0; sectorIndex < 8; sectorIndex++)
	{
		AABB childBounds = sectorBounds;
		OctTreeNode* childNode = m_octTree->TraverseIntoSector(node, sectorIndex, childBounds);
		if (childNode != nullptr)
			RenderSectorContours(childNode, childBounds, depth + 1);
	}
}

void OctTreeRenderer::DrawSector(const AABB& bounds)
{
	// Render a wire-frame cube.

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

void OctTreeRenderer::DrawCircle(const Bounds2D& circle,
								int axis, float posOnAxis)
{
	int xAxis = (axis + 1) % 3;
	int yAxis = (axis + 2) % 3;

	Vector3f vertex;
	vertex[axis] = posOnAxis;

	// Render a circle contour.
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 20; i++)
	{
		float angle = (i / 20.0f) * Math::TWO_PI;
		vertex[xAxis] = circle.center.x + cos(angle) * circle.radius;
		vertex[yAxis] = circle.center.y - sin(angle) * circle.radius;
		glVertex3fv(vertex.data());
	}
	glEnd();
}

void OctTreeRenderer::DrawArc(	Vector2f a, Vector2f b,
								const Bounds2D& circle,
								int axis, float posOnAxis)
{
	int xAxis = (axis + 1) % 3;
	int yAxis = (axis + 2) % 3;

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
	Vector3f vertex;
	vertex[axis] = posOnAxis;

	glBegin(GL_LINE_STRIP);

	Vector2f v2 = a;
	vertex[xAxis] = a.x;
	vertex[yAxis] = a.y;
	glVertex3fv(vertex.data());

	for (int i = 0; i < numSteps; i++)
	{
		v2 = rotation.TransformVector(v2);
		vertex[xAxis] = v2.x;
		vertex[yAxis] = v2.y;
		glVertex3fv(vertex.data());
	}

	vertex[xAxis] = b.x;
	vertex[yAxis] = b.y;
	glVertex3fv(vertex.data());

	glEnd();
}
	

//-----------------------------------------------------------------------------
// Contour calculation
//-----------------------------------------------------------------------------

int OctTreeRenderer::PerformContour(const Bounds2D& circle,
		const Bounds2D& square, CircleContour* contours, int& outNumContours)
{
	// Check if the square fully contains the circle.
	if (square.SquareContainsSquare(circle))
	{
		outNumContours = 1;
		contours[0].fullCircle = true;
		return 1;
	}
	
	outNumContours = 0;

	Vector2f corners[4];
	bool circleContainsCorner[4];
	bool squareContainsCorner[4];

	// Compute square corner locations.
	corners[0] = Vector2f(square.center.x - square.radius, square.center.y - square.radius);
	corners[1] = Vector2f(square.center.x - square.radius, square.center.y + square.radius);
	corners[2] = Vector2f(square.center.x + square.radius, square.center.y + square.radius);
	corners[3] = Vector2f(square.center.x + square.radius, square.center.y - square.radius);

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
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i, 0, corners),
				ComputeIntersectionPoint(circle, i - 1, 1, corners));
		}

		// Opposite edge case.
		if (squareContainsCorner[currCornerIndex] &&
			squareContainsCorner[nextCornerIndex] &&
			!squareContainsCorner[prevCornerIndex] &&
			(onInside || (circleContainsCorner[currCornerIndex] &&
				circleContainsCorner[nextCornerIndex])))
		{
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i + 1, 1, corners));
		}

		// Adjacent edge case.
		else if (squareContainsCorner[currCornerIndex] &&
			((!squareContainsCorner[prevCornerIndex] || !onInside) &&
				!circleContainsCorner[prevCornerIndex]) &&
			((!squareContainsCorner[nextCornerIndex] || !onInside) &&
				!circleContainsCorner[nextCornerIndex]) &&
			!circleBoundsContainsSquare &&
			(onInside || circleContainsCorner[currCornerIndex]))
		{
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i, 1, corners));
		}
	}
	
	if (outNumContours == 0)
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
				contours[outNumContours++] = CircleContour(
					ComputeIntersectionPoint(circle, i, 0, corners),
					ComputeIntersectionPoint(circle, i, 1, corners));
				break;
			}
		}
	}

	return outNumContours;
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
