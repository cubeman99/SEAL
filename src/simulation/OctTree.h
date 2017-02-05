#ifndef _OCT_TREE_H_
#define _OCT_TREE_H_

#include "SimulationObject.h"
#include <math/Vector3f.h>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------
// OctTreeNode
//-----------------------------------------------------------------------------
class OctTreeNode
{
public:
	typedef std::vector<SimulationObject*> object_list;
	friend class OctTree;

public:
	OctTreeNode();

	bool HasAnyChildNodes() const;
	unsigned int GetObjectCount() const;
	object_list::iterator objects_begin();
	object_list::iterator objects_end();

private:
	unsigned char m_sectorIndex;
	OctTreeNode* m_parent;
	OctTreeNode* m_children[8];
	object_list m_objects;
};


//-----------------------------------------------------------------------------
// OctTree
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

	// TODO: describe this or rename it.
	OctTreeNode* TraverseIntoSector(OctTreeNode* node,
		unsigned int sectorIndex, AABB& bounds);
	
	//-------------------------------------------------------------------------
	// Queries

	// Query for objects which are touching the given sphere,
	// This needs a callback function that takes a single
	// SimulationObject* as a parameter.
	template <class T_QueryCallback>
	void Query(const Sphere& sphere, T_QueryCallback callback);
	
	// Query for objects which are touching the given box,
	// This needs a callback function that takes a single
	// SimulationObject* as a parameter.
	template <class T_QueryCallback>
	void Query(const AABB& box, T_QueryCallback callback);


private:
	//-------------------------------------------------------------------------
	// Private functions

	void DoGetNumObjects(const OctTreeNode* node, unsigned int& count) const;
	
	void DoClear(OctTreeNode* node);
	
	OctTreeNode* DoGetNode(OctTreeNode* node, const Vector3f& point,
		AABB& bounds, unsigned int& depth);
	
	unsigned int DoGetSectorIndex(const AABB& bounds, const Vector3f& point);
	unsigned int DoGetSectorIndex(const Vector3f& boundsCenter,
		const Vector3f& point);
	
	void SplitBoundsBySector(AABB& bounds, unsigned int sectorIndex);
	
	void DoRemoveNode(OctTreeNode* node);

	void DoInsertObjectIntoNode(object_pointer object, OctTreeNode* node,
		const AABB& bounds, unsigned int depth);
	
	template <class T_QueryCallback>
	void DoBoxQuery(OctTreeNode* sectorNode,
					const AABB& sectorBounds,
					const AABB& queryBounds,
					const AABB& box,
					T_QueryCallback callback);

	template <class T_QueryCallback>
	void DoSphereQuery(	OctTreeNode* sectorNode,
						const AABB& sectorBounds,
						const AABB& queryBounds,
						const Sphere& sphere,
						T_QueryCallback callback);

private:
	//-------------------------------------------------------------------------
	// Member variables

	OctTreeNode		m_root;
	AABB			m_bounds;
	unsigned int	m_maxDepth;				// Maximum number of subdivisions
	unsigned int	m_maxObjectsPerNode;	// Max objects per node before a division happens
	ObjectToNodeMap	m_objectToNodeMap;
	float			m_largestObjectRadius;	// Keeps track of the largest object radius in the tree.
};


//-----------------------------------------------------------------------------
// OctTree template method definitions
//-----------------------------------------------------------------------------

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

		if (box.Intersects(objectSphere))
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

		if (sphere.Intersects(objectSphere))
		{
			callback(object);
		}
	}
}


#endif // _OCT_TREE_H_