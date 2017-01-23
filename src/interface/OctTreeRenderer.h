#ifndef _OCT_TREE_RENDERER_H_
#define _OCT_TREE_RENDERER_H_

#include <simulation/OctTree.h>
#include <graphics/Renderer.h>
#include "ResourceManager.h"
#include <math/MathLib.h>
#include "SimulationManager.h"


struct CircleContour
{
	Vector2f startPoint;
	Vector2f endPoint;
	bool fullCircle;

	CircleContour() :
		fullCircle(false)
	{}

	CircleContour(const Vector2f& startPoint, const Vector2f& endPoint) :
		startPoint(startPoint),
		endPoint(endPoint),
		fullCircle(false)
	{}

	CircleContour(bool fullCircle) :
		fullCircle(fullCircle)
	{}
};


struct Bounds2D
{
	Vector2f center;
	float radius;

	bool CircleContains(const Vector2f& point) const
	{
		return (point.DistToSqr(center) < radius * radius);
	}
	
	bool SquareContains(const Vector2f& point) const
	{
		return (Math::Abs(point.x - center.x) < radius &&
				Math::Abs(point.y - center.y) < radius);
	}
	
	bool SquareContainsSquare(const Bounds2D& bounds) const
	{
		return (bounds.center.x - bounds.radius >= center.x - radius &&
				bounds.center.x + bounds.radius <= center.x + radius &&
				bounds.center.y - bounds.radius >= center.y - radius &&
				bounds.center.y + bounds.radius <= center.y + radius);
	}
	
	bool SquareIntersectsSquare(const Bounds2D& bounds) const
	{
		return (center.x + radius > bounds.center.x - bounds.radius &&
				bounds.center.x + bounds.radius > center.x - radius &&
				center.y + radius > bounds.center.y - bounds.radius &&
				bounds.center.y + bounds.radius > center.y - radius);
	}
};


class OctTreeRenderer
{
public:
	OctTreeRenderer();

	void Initialize(ResourceManager* resourceManager, SimulationManager* simulationManager);

	void RenderOctTree(Renderer* renderer, OctTree* octTree);

	void RenderWireFrame();
	void RenderSurfaceContours();

private:
	void RenderOctTreeNode(OctTreeNode* node, const AABB& bounds, int depth, bool lines);
	void DrawCircle(const Bounds2D& circle, int axis, float posOnAxis);
	void DrawArc(Vector2f a, Vector2f b, const Bounds2D& circle, int axis, float posOnAxis);
	void DrawSector(const AABB& bounds);

	Vector2f ComputeIntersectionPoint(const Bounds2D& circle, int edgeIndex, int pointIndex, Vector2f* corners);
	int PerformContour(const Bounds2D& circle, const Bounds2D& square, CircleContour* contours, int& outNumContours);

	SimulationManager* m_simulationManager;
	Renderer*	m_renderer;
	OctTree*	m_octTree;
	Shader*		m_shaderUnlit;
};


#endif // _OCT_TREE_RENDERER_H_