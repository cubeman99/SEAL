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
	// Constructor/destructor
	OctTree();
	~OctTree();

	// Getters
	inline const AABB& GetBounds() const { return m_bounds; }
	inline const OctTreeNode* GetRootNode() const { return &m_root; }
	inline OctTreeNode* GetRootNode() { return &m_root; }
	unsigned int GetNumObjects() const;
	inline unsigned int GetMaxDepth() const { return m_maxDepth; }
	
	// Setters
	inline void SetMaxDepth(unsigned int maxDepth) { m_maxDepth = maxDepth; }
	inline void SetMaxObjectsPerNode(unsigned int maxObjectsPerNode) { m_maxObjectsPerNode = maxObjectsPerNode; }
	inline void SetBounds(const AABB& bounds) { m_bounds = bounds; }

	// Operations
	void Clear();
	void InsertObject(object_pointer object);
	void RemoveObject(object_pointer object);
	void DynamicUpdate(object_pointer object);
	OctTreeNode* TraverseIntoSector(OctTreeNode* node, unsigned int sectorIndex, AABB& bounds);
	
	// TODO: Queries
	//template <T>
	//void QueryCollision(const Sphere& sphere);
	

private:

	// Private functions
	void DoGetNumObjects(const OctTreeNode* node, unsigned int& count) const;
	void DoClear(OctTreeNode* node);
	OctTreeNode* DoGetNode(OctTreeNode* node, const Vector3f& point, AABB& bounds, unsigned int& depth);
	unsigned int DoGetSectorIndex(const AABB& bounds, const Vector3f& point);
	unsigned int DoGetSectorIndex(const Vector3f& boundsCenter, const Vector3f& point);
	void SplitBoundsBySector(AABB& bounds, unsigned int sectorIndex);
	void DoRemoveNode(OctTreeNode* node);
	void DoInsertObjectIntoNode(object_pointer object, OctTreeNode* node, const AABB& bounds, unsigned int depth);

	OctTreeNode		m_root;
	AABB			m_bounds;
	unsigned int	m_maxDepth;				// Maximum number of subdivisions
	unsigned int	m_maxObjectsPerNode;	// Max objects per node before a division happens
	ObjectToNodeMap	m_objectToNodeMap;
	float			m_largestObjectRadius;	// Keeps track of the largest object radius in the tree.
};

	
/*
//typedef std::allocator<SimulationObject*> allocator_type;
//typedef allocator_type::value_type value_type; 
//typedef allocator_type::reference reference;
//typedef allocator_type::const_reference const_reference;
//typedef allocator_type::difference_type difference_type;
//typedef allocator_type::size_type size_type;

class iterator
{
public:
	typedef std::allocator<SimulationObject*> allocator_type;
    typedef allocator_type::difference_type difference_type;
    typedef allocator_type::value_type value_type;
    typedef allocator_type::reference reference;
    typedef allocator_type::pointer pointer;
    //typedef std::random_access_iterator_tag iterator_category; //or another tag

	iterator();
	iterator(const iterator&);
	~iterator();

	iterator& operator=(const iterator&);
	bool operator==(const iterator&) const;
	bool operator!=(const iterator&) const;
	//bool operator<(const iterator&) const; //optional
	//bool operator>(const iterator&) const; //optional
	//bool operator<=(const iterator&) const; //optional
	//bool operator>=(const iterator&) const; //optional

	iterator& operator++();
	iterator operator++(int); //optional
	//iterator& operator--(); //optional
	//iterator operator--(int); //optional
	//iterator& operator+=(size_type); //optional
	//iterator operator+(size_type) const; //optional
	//friend iterator operator+(size_type, const iterator&); //optional
	//iterator& operator-=(size_type); //optional            
	//iterator operator-(size_type) const; //optional
	//difference_type operator-(iterator) const; //optional

	reference operator*() const;
	pointer operator->() const;
	reference operator[](size_type) const; //optional
};*/


#endif // _OCT_TREE_H_