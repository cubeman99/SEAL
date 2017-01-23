#ifndef _OCT_TREE_RENDERER_H_
#define _OCT_TREE_RENDERER_H_

#include <simulation/OctTree.h>
#include <graphics/Renderer.h>
#include "ResourceManager.h"


class OctTreeRenderer
{
public:
	OctTreeRenderer();

	void Initialize(ResourceManager* resourceManager);

	void RenderOctTree(Renderer* renderer, OctTree* octTree);

private:
	void RenderOctTreeNode(OctTree* octTree, OctTreeNode* node, const AABB& bounds, int depth, bool lines);

	Renderer*	m_renderer;
	OctTree*	m_octTree;
	Shader*		m_shaderUnlit;
	Shader*		m_shaderOctTree;
};


#endif // _OCT_TREE_RENDERER_H_