#include "OctTreeRenderer.h"
#include <graphics/OpenGLIncludes.h>


OctTreeRenderer::OctTreeRenderer() :
	m_octTree(nullptr),
	m_renderer(nullptr),
	m_shaderUnlit(nullptr)
{
}

void OctTreeRenderer::Initialize(ResourceManager* resourceManager)
{
	m_shaderUnlit = resourceManager->GetShader("unlit");
	
	m_shaderOctTree = resourceManager->LoadShader("octtree",
		"../../assets/shaders/octtree_vs.glsl",
		"../../assets/shaders/octtree_fs.glsl");
}

void OctTreeRenderer::RenderOctTree(Renderer* renderer, OctTree* octTree)
{
	m_renderer = renderer;
	m_octTree = octTree;

	// Adjust render params.
	RenderParams renderParams = m_renderer->GetRenderParams();
	renderParams.EnableCullFace(false);
	m_renderer->SetRenderParams(renderParams);
	m_renderer->ApplyRenderSettings(false);

	// Create a material for the oct-tree.
	Material octTreeMaterial;
	octTreeMaterial.SetColor(Color::YELLOW);
	octTreeMaterial.SetIsLit(false);
		
	// Render the wire frame of the oct-tree.
	octTreeMaterial.SetColor(Color::YELLOW * 0.5f);
	m_renderer->SetShader(m_shaderUnlit);
	m_renderer->UpdateUniforms(&octTreeMaterial, Matrix4f::IDENTITY);
	glBegin(GL_LINES);
		RenderOctTreeNode(octTree, octTree->GetRootNode(), octTree->GetBounds(), 0, true);
	glEnd();
	
	// Render the intersection contours between the oct-tree and the world sphere.
	octTreeMaterial.SetColor(Color::YELLOW);
	m_renderer->SetShader(m_shaderOctTree);
	m_renderer->UpdateUniforms(&octTreeMaterial, Matrix4f::IDENTITY);
	glBegin(GL_QUADS);
		RenderOctTreeNode(octTree, octTree->GetRootNode(), octTree->GetBounds(), 0, false);
	glEnd();
}




void OctTreeRenderer::RenderOctTreeNode(OctTree* octTree, OctTreeNode* node, const AABB& bounds, int depth, bool lines)
{
	if (lines)
	{
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
	}
	else
	{
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z); // bottom
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z); // top
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z); // front
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z); // back
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z); // right
		glVertex3f(bounds.maxs.x, bounds.maxs.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.mins.z);
		glVertex3f(bounds.maxs.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.mins.z); // left
		glVertex3f(bounds.mins.x, bounds.maxs.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.maxs.z);
		glVertex3f(bounds.mins.x, bounds.mins.y, bounds.mins.z);
	}

	// Recursively render child nodes.
	for (int sectorIndex = 0; sectorIndex < 8; sectorIndex++)
	{
		AABB childBounds = bounds;
		OctTreeNode* childNode = octTree->TraverseIntoSector(node, sectorIndex, childBounds);
		if (childNode != nullptr)
			RenderOctTreeNode(octTree, childNode, childBounds, depth + 1, lines);
	}
}


