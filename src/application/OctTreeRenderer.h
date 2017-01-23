#ifndef _OCT_TREE_RENDERER_H_
#define _OCT_TREE_RENDERER_H_

#include <simulation/OctTree.h>
#include <graphics/Renderer.h>
#include "ResourceManager.h"
#include <math/MathLib.h>

class SimulationManager;



//-----------------------------------------------------------------------------
// Bounds2D - Used to represent either a square or a circle.
//-----------------------------------------------------------------------------
struct Bounds2D
{
public:
	Vector2f	center;
	float		radius;

	bool CircleContains(const Vector2f& point) const;
	bool SquareContains(const Vector2f& point) const;
	bool SquareContainsSquare(const Bounds2D& bounds) const;
	bool SquareIntersectsSquare(const Bounds2D& bounds) const;
};


//-----------------------------------------------------------------------------
// SectorFace - A square face for a sector cube.
//-----------------------------------------------------------------------------
struct SectorFace
{
public:
	int			axis;		// The axis perpendicular to the face.
	float		posOnAxis;	// The position of the face along the axis.
	Bounds2D	rectOnFace; // The 2D rectangle on the plane of the face

	// Get the 3D point corresponding to the given
	// 2D point along this face's plane.
	Vector3f Get3DPoint(const Vector2f& pointOnFacePlane) const;
};


//-----------------------------------------------------------------------------
// ContourCurve - A single contour to render (as either an arc or circle).
//-----------------------------------------------------------------------------
struct ContourCurve
{
public:
	Vector2f	startPoint;
	Vector2f	endPoint;
	bool		fullCircle;

	ContourCurve();
	ContourCurve(const Vector2f& startPoint, const Vector2f& endPoint);
	ContourCurve(bool fullCircle);
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
	void RenderWireFrame(const Color& color);
	void RenderSurfaceContours(const Color& color);

private:
	// Wireframe rendering
	void RenderSectorBoxes(OctTreeNode* node, const AABB& bound, int depth);
	void DrawSectorWireFrame(const AABB& sectorBounds);

	// Contour rendering
	void RenderContoursForSector(OctTreeNode* node,
								const AABB& sectorBounds,
								int depth);
	unsigned int RenderContoursForFace(const SectorFace& face);
	void DrawContour(const SectorFace& face,
					const Bounds2D& circle,
					const ContourCurve& contour);
	void DrawCircleContour(const SectorFace& face,
							const Bounds2D& circle);
	void DrawArcContour(const SectorFace& face,
						const Bounds2D& circle,
						Vector2f a,
						Vector2f b);

	// Contour calculation
	unsigned int GenerateContours(const Bounds2D& circle,
									const Bounds2D& square,
									ContourCurve* contours);
	Vector2f ComputeIntersectionPoint(	const Bounds2D& circle, int edgeIndex,
										int pointIndex, Vector2f* corners);


	SimulationManager* m_simulationManager;
	Renderer*	m_renderer;
	OctTree*	m_octTree;
	Shader*		m_shaderUnlit;
	Sphere		m_worldSphere;
};


#endif // _OCT_TREE_RENDERER_H_