#include "OctTree.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// OctTreeNode
//-----------------------------------------------------------------------------

OctTreeNode::OctTreeNode() :
	m_parent(nullptr)
{
	for (unsigned int i = 0; i < 8; ++i)
		m_children[i] = nullptr;
}

bool OctTreeNode::HasAnyChildNodes() const
{
	for (unsigned int i = 0; i < 8; ++i)
	{
		if (m_children[i] != nullptr)
			return true;
	}
	return false;
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


//-----------------------------------------------------------------------------
// OctTree constructor/destructor
//-----------------------------------------------------------------------------

OctTree::OctTree() :
	m_bounds(Vector3f(-1,-1,-1), Vector3f(1,1,1)),
	m_maxDepth(4),
	m_maxObjectsPerNode(2),
	m_largestObjectRadius(0.0f)
{
}

OctTree::~OctTree()
{
	Clear();
}


//-----------------------------------------------------------------------------
// OctTree getters
//-----------------------------------------------------------------------------

unsigned int OctTree::GetNumObjects() const
{
	unsigned int count = 0;
	DoGetNumObjects(&m_root, count);
	return count;
}


//-----------------------------------------------------------------------------
// OctTree operations
//-----------------------------------------------------------------------------

void OctTree::Clear()
{
	DoClear(&m_root);
	m_objectToNodeMap.clear();
	m_largestObjectRadius = 0.0f;
}

void OctTree::InsertObject(object_pointer object)
{
	// Get the leaf node for the point.
	Vector3f point = object->GetPosition();
	AABB bounds = m_bounds;
	unsigned int depth = 0;
	OctTreeNode* node = DoGetNode(&m_root, point, bounds, depth);

	// Insert the object recursively into the leaf node.
	DoInsertObjectIntoNode(object, node, bounds, depth);
}

void OctTree::RemoveObject(object_pointer object)
{
	OctTreeNode* node = m_objectToNodeMap[object];

	// Delete the node if this is the only object in it.
	if (node->GetObjectCount() == 1 && node != &m_root)
	{
		node->m_objects.clear();
		DoRemoveNode(node);
	}
	else
	{
		// Remove the object from the node.
		auto it = std::find(node->m_objects.begin(), node->m_objects.end(), object);
		if (it != node->m_objects.end())
			node->m_objects.erase(it);
	}

	m_objectToNodeMap.erase(object);
}

void OctTree::DynamicUpdate(object_pointer object)
{
	// Get the object's current node.
	OctTreeNode* currentNode = m_objectToNodeMap[object];
	
	// Determine the node for the object's updated position.
	Vector3f point = object->GetPosition();
	AABB bounds = m_bounds;
	unsigned int depth = 0;
	OctTreeNode* newNode = DoGetNode(&m_root, point, bounds, depth);

	// Dynamically update the tree if the object has left its current node.
	if (newNode != currentNode)
	{
		// TODO: this is pretty naive, find a more efficient way to do this.
		RemoveObject(object);
		InsertObject(object);
	}
}

OctTreeNode* OctTree::TraverseIntoSector(OctTreeNode* node,
										unsigned int sectorIndex,
										AABB& bounds)
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


//-----------------------------------------------------------------------------
// OctTree private methods
//-----------------------------------------------------------------------------

void OctTree::DoGetNumObjects(	const OctTreeNode* node,
								unsigned int& count) const
{
	count += node->GetObjectCount();

	// Recursively count the child nodes.
	for (unsigned int sectorIndex = 0; sectorIndex < 8; ++sectorIndex)
	{
		if (node->m_children[sectorIndex] != nullptr)
			DoGetNumObjects(node->m_children[sectorIndex], count);
	}
}

void OctTree::DoClear(OctTreeNode* node)
{
	// Recursively clear each child sector in this node.
	for (unsigned int sectorIndex = 0; sectorIndex < 8; ++sectorIndex)
	{
		if (node->m_children[sectorIndex] != nullptr)
		{
			DoClear(node->m_children[sectorIndex]);
			node->m_children[sectorIndex] = nullptr;
		}
	}

	// Delete this node itself (but not the root node).
	if (node != &m_root)
		delete node;
}

OctTreeNode* OctTree::DoGetNode(OctTreeNode* node,
								const Vector3f& point,
								AABB& bounds,
								unsigned int& depth)
{
	unsigned int sectorIndex = DoGetSectorIndex(bounds, point);

	if (node->m_children[sectorIndex] != nullptr)
	{
		// Step into the child node.
		depth += 1;
		SplitBoundsBySector(bounds, sectorIndex);
		return DoGetNode(node->m_children[sectorIndex],
			point, bounds, depth);
	}
	else
	{
		// We have reached a leaf node.
		return node;
	}
}

unsigned int OctTree::DoGetSectorIndex(	const AABB& bounds,
										const Vector3f& point)
{
	return DoGetSectorIndex(bounds.GetCenter(), point);
}

unsigned int OctTree::DoGetSectorIndex(	const Vector3f& boundsCenter,
										const Vector3f& point)
{
	// Sector indices are a bit map per axis.
	// A set bit means positive on that axis.
	// The axis bits are orded like so:
	//   bit 0: X, bit 1: Y, bit 2: Z

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

void OctTree::DoRemoveNode(OctTreeNode* node)
{
	// Don't remove nodes that have child nodes.
	for (unsigned int sectorIndex = 0; sectorIndex < 8; ++sectorIndex)
	{
		if (node->m_children[sectorIndex] != nullptr)
			return;
	}
	
	// Only remove nodes that have no objects.
	if (node->m_objects.empty())
	{
		node->m_parent->m_children[node->m_sectorIndex] = nullptr;
		DoRemoveNode(node->m_parent);
		delete node;
	}
}

void OctTree::DoInsertObjectIntoNode(object_pointer object,
									OctTreeNode* node,
									const AABB& bounds,
									unsigned int depth)
{
	// Get the leaf node for the point.
	Vector3f point = object->GetPosition();

	// Check if we should create a new child node.
	if (depth < m_maxDepth && (node->HasAnyChildNodes() ||
		node->GetObjectCount() >= m_maxObjectsPerNode))
	{
		// TEMP: put the new object into this node so the following code works.
		node->m_objects.push_back(object);

		// Put the existing objects in this node into new child nodes.
		for (auto it = node->objects_begin(); it != node->objects_end(); ++it)
		{
			object_pointer obj = *it;
			unsigned int sectorIndex = DoGetSectorIndex(bounds, obj->GetPosition());

			// We may need to instantiate a new child node.
			if (node->m_children[sectorIndex] == nullptr)
			{
				node->m_children[sectorIndex] = new OctTreeNode();
				node->m_children[sectorIndex]->m_parent = node;
				node->m_children[sectorIndex]->m_sectorIndex = (unsigned char) sectorIndex;
			}
		
			// Recursively insert the object into this child node.
			AABB childBounds = bounds;
			SplitBoundsBySector(childBounds, sectorIndex);
			DoInsertObjectIntoNode(obj, node->m_children[sectorIndex], childBounds, depth + 1);
		}
		
		node->m_objects.clear();
	}
	else
	{
		// Add the object to this node.
		node->m_objects.push_back(object);
		m_objectToNodeMap[object] = node;
	}

	// Keep track of the largest object radius in the tree.
	if (object->GetRadius() > m_largestObjectRadius)
		m_largestObjectRadius = object->GetRadius();
}

