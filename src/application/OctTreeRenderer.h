#ifndef _OCT_TREE_RENDERER_H_
#define _OCT_TREE_RENDERER_H_

#include <simulation/OctTree.h>
#include <graphics/Renderer.h>
#include "ResourceManager.h"
#include <math/MathLib.h>

class SimulationManager;



//-----------------------------------------------------------------------------
// CircleContour - A single contour to render (as either an arc or circle).
//-----------------------------------------------------------------------------
struct CircleContour
{
public:
	Vector2f startPoint;
	Vector2f endPoint;
	bool fullCircle;

	CircleContour();
	CircleContour(const Vector2f& startPoint, const Vector2f& endPoint);
	CircleContour(bool fullCircle);
};


//-----------------------------------------------------------------------------
// Bounds2D - Used to represent either a square or a circle.
//-----------------------------------------------------------------------------
struct Bounds2D
{
public:
	Vector2f center;
	float radius;

	bool CircleContains(const Vector2f& point) const;
	bool SquareContains(const Vector2f& point) const;
	bool SquareContainsSquare(const Bounds2D& bounds) const;
};


//-----------------------------------------------------------------------------
// OctTreeRenderer - Renders OctTrees
//-----------------------------------------------------------------------------
class OctTreeRenderer
{
public:
	OctTreeRenderer();

	void Initialize(ResourceManager* resourceManager,
					SimulationManager* simulationManager);

	// Rendering
	void RenderOctTree(Renderer* renderer, OctTree* octTree);
	void RenderWireFrame();
	void RenderSurfaceContours();

private:
	// Private render methods
	void RenderSectorBoxes(OctTreeNode* node, const AABB& bound, int depth);
	void RenderSectorContours(OctTreeNode* node,
							const AABB& sectorBound,
							int depth);

	void DrawCircle(const Bounds2D& circle, int axis, float posOnAxis);
	void DrawArc(Vector2f a, Vector2f b, const Bounds2D& circle,
				int axis, float posOnAxis);
	void DrawSector(const AABB& bounds);

	// Contour calculation
	int PerformContour(	const Bounds2D& circle, const Bounds2D& square,
						CircleContour* contours, int& outNumContours);
	Vector2f ComputeIntersectionPoint(	const Bounds2D& circle, int edgeIndex,
										int pointIndex, Vector2f* corners);


	SimulationManager* m_simulationManager;
	Renderer*	m_renderer;
	OctTree*	m_octTree;
	Shader*		m_shaderUnlit;
	Sphere		m_worldSphere;
};


#endif // _OCT_TREE_RENDERER_H_