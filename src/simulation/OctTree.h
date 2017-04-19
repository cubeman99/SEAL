#ifndef _OCT_TREE_H_
#define _OCT_TREE_H_

#include "SimulationObject.h"
#include <math/Vector3f.h>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------
// OctTreeNode - A single node in an octtree
//-----------------------------------------------------------------------------
class OctTreeNode
{
public:
	friend class OctTree;
	typedef std::vector<SimulationObject*> object_list;

public:
	OctTreeNode();

	bool HasAnyChildNodes() const;
	unsigned int GetObjectCount() const;
	object_list::iterator objects_begin();
	object_list::iterator objects_end();

private:
	unsigned char	m_sectorIndex;	// Which sector is this node in its parent?
	OctTreeNode*	m_parent;		// The parent node
	OctTreeNode*	m_children[8];	// The 8 child nodes
	object_list		m_objects;		// The objects contained in this node (for leaf nodes)
};


//-----------------------------------------------------------------------------
// OctTree - Spatial partitioning data structure that stores and sorts
//           simulation objects by position in a box-volume tree heirerchery
//           that is recursively subdivided into 8 sectors. This speeds up
//           queries for objects based on their position.
//-----------------------------------------------------------------------------
class OctTree
{
public:
	typedef SimulationObject	object_type;
	typedef object_type*		object_pointer;
	typedef std::map<object_pointer, OctTreeNode*> ObjectToNodeMap;

public:
	//-------------------------------------------------------------------------
	// Constructor/destructor

	OctTree();
	~OctTree();

	//-------------------------------------------------------------------------
	// Getters

	inline const AABB& GetBounds() const { return m_bounds; }
	inline const OctTreeNode* GetRootNode() const { return &m_root; }
	inline OctTreeNode* GetRootNode() { return &m_root; }
	inline unsigned int GetMaxDepth() const { return m_maxDepth; }
	unsigned int GetNumObjects() const;
	
	//-------------------------------------------------------------------------
	// Setters

	inline void SetMaxDepth(unsigned int maxDepth) { m_maxDepth = maxDepth; }
	inline void SetMaxObjectsPerNode(unsigned int maxObjectsPerNode)
		{ m_maxObjectsPerNode = maxObjectsPerNode; }
	inline void SetBounds(const AABB& bounds) { m_bounds = bounds; }
	
	//-------------------------------------------------------------------------
	// Operations

	// Clear all objects from the octtree.
	void Clear();

	// Insert a new object into the octtree.
	void InsertObject(object_pointer object);

	// Remove an object from the octtree.
	void RemoveObject(object_pointer object);

	// Dynamically update the octtree for the given object, reshaping
	// the tree structure if the object has moved.
	void DynamicUpdate(object_pointer object);

	// Get the child node and bounds of an octtree node.
	OctTreeNode* TraverseIntoSector(OctTreeNode* node,
		unsigned int sectorIndex, AABB& bounds);
	
	//-------------------------------------------------------------------------
	// Queries
	
	// Query for objects which are touching the given box, This needs a
	// callback function that takes a single SimulationObject* as a parameter.
	template <class T_QueryCallback>
	void Query(const AABB& box, T_QueryCallback callback);

	// Query for objects which are touching the given sphere, This needs a
	// callback function that takes a single SimulationObject* as a parameter.
	template <class T_QueryCallback>
	void Query(const Sphere& sphere, T_QueryCallback callback);


private:
	//-------------------------------------------------------------------------
	// Private functions

	// Recursively count the number of objects in a node and its children.
	void DoGetNumObjects(const OctTreeNode* node, unsigned int& count) const;
	
	// Recursively remove a node and its objects, moving down the tree.
	void DoClear(OctTreeNode* node);
	
	// Recursively remove a node if it has no objects, moving up the tree.
	void DoRemoveNode(OctTreeNode* node);
	
	// Recursively find the leaf node that an object would fall into.
	OctTreeNode* DoGetNode(OctTreeNode* node, const Vector3f& point,
		AABB& bounds, unsigned int& depth);
	
	// Within the given bounds, return the sector index that a point
	// woudld fall into.
	unsigned int DoGetSectorIndex(const AABB& bounds, const Vector3f& point);

	// Given the center point of a bounds, return the sector index that a
	// point woudld fall into.
	unsigned int DoGetSectorIndex(const Vector3f& boundsCenter,
		const Vector3f& point);
	
	// Get the subdivided-bounds for a given sector index.
	void SplitBoundsBySector(AABB& bounds, unsigned int sectorIndex);

	// Recursively insert an object into a node.
	void DoInsertObjectIntoNode(object_pointer object, OctTreeNode* node,
		const AABB& bounds, unsigned int depth);
	
	// Recursively perform a box query.
	template <class T_QueryCallback>
	void DoBoxQuery(OctTreeNode* sectorNode,
					const AABB& sectorBounds,
					const AABB& queryBounds,
					const AABB& box,
					T_QueryCallback callback);
	
	// Recursively perform a sphere query.
	template <class T_QueryCallback>
	void DoSphereQuery(	OctTreeNode* sectorNode,
						const AABB& sectorBounds,
						const AABB& queryBounds,
						const Sphere& sphere,
						T_QueryCallback callback);

private:
	//-------------------------------------------------------------------------
	// Member variables

	OctTreeNode		m_root;					// The root node of the tree
	AABB			m_bounds;				// The entire space that this tree encompasses
	unsigned int	m_maxDepth;				// Maximum number of subdivisions
	unsigned int	m_maxObjectsPerNode;	// Max number of objects per node before a sub-division happens (increasing depth)
	ObjectToNodeMap	m_objectToNodeMap;		// Maps objects to the nodes in which they're contained
	float			m_largestObjectRadius;	// Keeps track of the largest object radius in the tree
};


//-----------------------------------------------------------------------------
// OctTree template method definitions
//-----------------------------------------------------------------------------

// Query for objects which are touching the given box, This needs a callback
// function that takes a single SimulationObject* as a parameter.
template <class T_QueryCallback>
void OctTree::Query(const AABB& box, T_QueryCallback callback)
{
	// Create an AABB that represents the bounds were are querying with.
	// Object positions must be contained within these bounds to pass the query.
	AABB queryBounds = box;
	Vector3f inflation(m_largestObjectRadius);
	queryBounds.mins -= inflation;
	queryBounds.maxs += inflation;

	// Recursively perform the query.
	DoBoxQuery(&m_root, m_bounds, queryBounds, box, callback);
}

// Query for objects which are touching the given sphere, This needs a
// callback function that takes a single SimulationObject* as a parameter.
template <class T_QueryCallback>
void OctTree::Query(const Sphere& sphere, T_QueryCallback callback)
{
	// Create an AABB that represents the bounds were are querying with.
	// Object positions must be contained within these bounds to pass the query.
	AABB queryBounds;
	Vector3f halfSize(sphere.radius + m_largestObjectRadius);
	queryBounds.mins = sphere.position - halfSize;
	queryBounds.maxs = sphere.position + halfSize;

	// Recursively perform the query.
	DoSphereQuery(&m_root, m_bounds, queryBounds, sphere, callback);
}

template <class T_QueryCallback>
void OctTree::DoBoxQuery(OctTreeNode* sectorNode,
							const AABB& sectorBounds,
							const AABB& queryBounds,
							const AABB& box,
							T_QueryCallback callback)
{
	// Recursively query the child nodes.
	for (unsigned char i = 0; i < 8; ++i)
	{
		if (sectorNode->m_children[i] != nullptr)
		{
			// Get the bounds for this child sector.
			AABB childSectorBounds = sectorBounds;
			SplitBoundsBySector(childSectorBounds, i);

			// Make sure the object is touching this node.
			if (queryBounds.Intersects(childSectorBounds))
			{
				DoBoxQuery(sectorNode->m_children[i],
					childSectorBounds, queryBounds, box, callback);
			}
		}
	}
	
	// Query the objects of this node.
	for (unsigned int i = 0; i < sectorNode->m_objects.size(); ++i)
	{
		SimulationObject* object = sectorNode->m_objects[i];
		Sphere objectSphere(object->GetPosition(), object->GetRadius());

		if (box.Intersects(objectSphere) && !object->IsDestroyed())
		{
			callback(object);
		}
	}
}

template <class T_QueryCallback>
void OctTree::DoSphereQuery(OctTreeNode* sectorNode,
							const AABB& sectorBounds,
							const AABB& queryBounds,
							const Sphere& sphere,
							T_QueryCallback callback)
{
	// Recursively query the child nodes.
	for (unsigned char i = 0; i < 8; ++i)
	{
		if (sectorNode->m_children[i] != nullptr)
		{
			// Get the bounds for this child sector.
			AABB childSectorBounds = sectorBounds;
			SplitBoundsBySector(childSectorBounds, i);

			// Make sure the object is touching this node.
			if (queryBounds.Intersects(childSectorBounds))
			{
				DoSphereQuery(sectorNode->m_children[i],
					childSectorBounds, queryBounds, sphere, callback);
			}
		}
	}
	
	// Query the objects of this node.
	for (unsigned int i = 0; i < sectorNode->m_objects.size(); ++i)
	{
		SimulationObject* object = sectorNode->m_objects[i];
		Sphere objectSphere(object->GetPosition(), object->GetRadius());

		if (sphere.Intersects(objectSphere) && !object->IsDestroyed())
		{
			callback(object);
		}
	}
}


#endif // _OCT_TREE_H_