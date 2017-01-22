#include "OctTree.h"


OctTreeNode::OctTreeNode()
{
	for (unsigned int i = 0; i < 8; i++)
		m_children[i] = nullptr;
}

bool OctTreeNode::HasAnyChildNodes() const
{
	for (unsigned int i = 0; i < 8; i++)
	{
		if (m_children[i] != nullptr)
			return true;
	}
	return false;
}

bool OctTreeNode::IsLeafNode() const
{
	return !HasAnyChildNodes();
}

unsigned int OctTreeNode::GetChildIndex(const Vector3f& point) const
{
	int index = 0;
	if (point.x > m_position.x)
		index |= 0x1;
	if (point.y > m_position.y)
		index |= 0x2;
	if (point.z > m_position.z)
		index |= 0x4;
	return index;
}

unsigned int OctTreeNode::GetObjectCount() const
{
	return m_objects.size();
}

OctTreeNode::object_list::iterator OctTreeNode::objects_begin()
{
	return m_objects.begin();
}

OctTreeNode::object_list::iterator OctTreeNode::objects_end()
{
	return m_objects.end();
}



OctTree::OctTree(const AABB& bounds, int maxDepth) :
	m_bounds(bounds),
	m_maxDepth(maxDepth),
	m_maxObjectsPerNode(2)
{
}

OctTree::~OctTree()
{
	Clear();
}

void OctTree::Clear()
{
	DoClear(&m_root);
}

void OctTree::InsertObject(object_pointer object)
{
	// Get the leaf node for the point.
	Vector3f point = object->GetPosition();
	AABB bounds = m_bounds;
	unsigned int depth = 0;
	OctTreeNode* node = DoGetNode(&m_root, point, bounds, depth);

	// Check if we should create a new child node.
	if (depth < m_maxDepth && (node->HasAnyChildNodes() ||
		node->GetObjectCount() >= m_maxObjectsPerNode))
	{
		unsigned int sectorIndex = DoGetSectorIndex(bounds, point);

		// We may need to instantiate a new child node.
		if (node->m_children[sectorIndex] == nullptr)
			node->m_children[sectorIndex] = new OctTreeNode();
		
		// Add the object to the child node.
		node->m_children[sectorIndex]->m_objects.push_back(object);
	}
	else
	{
		// Add the object to this node.
		node->m_objects.push_back(object);
	}
}


OctTreeNode* OctTree::TraverseIntoSector(OctTreeNode* node, unsigned int sectorIndex, AABB& bounds)
{
	if (node->m_children[sectorIndex])
	{
		SplitBoundsBySector(bounds, sectorIndex);
		return node->m_children[sectorIndex];
	}
	else
	{
		return nullptr;
	}
}

OctTreeNode* OctTree::DoGetNode(OctTreeNode* node, const Vector3f& point, AABB& bounds, unsigned int& depth)
{
	unsigned int sectorIndex = DoGetSectorIndex(bounds, point);

	if (node->m_children[sectorIndex] != nullptr)
	{
		// Step into the child node.
		depth += 1;
		SplitBoundsBySector(bounds, sectorIndex);
		return DoGetNode(node->m_children[sectorIndex], point, bounds, depth);
	}
	else
	{
		// We have reached a leaf node.
		return node;
	}
}

unsigned int OctTree::DoGetSectorIndex(const AABB& bounds, const Vector3f& point)
{
	return DoGetSectorIndex(bounds.GetCenter(), point);
}

unsigned int OctTree::DoGetSectorIndex(const Vector3f& boundsCenter, const Vector3f& point)
{
	int index = 0;
	if (point.x > boundsCenter.x)
		index |= 0x1;
	if (point.y > boundsCenter.y)
		index |= 0x2;
	if (point.z > boundsCenter.z)
		index |= 0x4;
	return index;
}

void OctTree::SplitBoundsBySector(AABB& bounds, unsigned int sectorIndex)
{
	Vector3f center = bounds.GetCenter();
	
	for (int axis = 0; axis < 3; axis++)
	{
		if (sectorIndex & (1 << axis))
			bounds.mins[axis] = center[axis];
		else
			bounds.maxs[axis] = center[axis];
	}
}



void OctTree::DoClear(OctTreeNode* node)
{
	// Clear each sector in this node.
	for (unsigned int sectorIndex = 0; sectorIndex < 8; ++sectorIndex)
	{
		if (node->m_children[sectorIndex] != nullptr)
			DoClear(node->m_children[sectorIndex]);
	}

	// Delete the node itself (but not the root node).
	if (node != &m_root)
		delete node;
}


