#ifndef _OCT_TREE_H_
#define _OCT_TREE_H_

#include <math/Vector3f.h>
#include <vector>


class SimulationObject
{
public:

};


enum OctTreeSector
{
	// bit 0: set = +X, clr = -X
	// bit 1: set = +Y, clr = -Y
	// bit 2: set = +Z, clr = -Z

	K_NX_NY_NZ = 0, // -X -Y -Z
	K_PX_NY_NZ = 1, // +X -Y -Z
	K_NX_PY_NZ = 2, // -X +Y -Z
	K_PX_PY_NZ = 3, // +X +Y -Z
	K_NX_NY_PZ = 4, // -X -Y +Z
	K_PX_NY_PZ = 5, // +X -Y +Z
	K_NX_PY_PZ = 6, // -X +Y +Z
	K_PX_PY_PZ = 7, // +X +Y +Z

};


enum OctTreeSectorBits
{
	k_x = 0x1,
	k_y = 0x2,
	k_z = 0x4,
};


class OctTreeNode
{
	typedef std::vector<SimulationObject*> object_list;

public:
	OctTreeNode();

	unsigned int GetChildIndex(const Vector3f& point);

	object_list::iterator objects_begin();
	object_list::iterator objects_end();

private:
	Vector3f m_position;
	OctTreeNode* m_children[8];
	object_list m_objects;
};

// TODO: OctTree object iterator


class OctTree
{
public:
	OctTree();

	typedef SimulationObject object_type;
	typedef object_type* object_pointer;
	typedef float bounding_volume_type;


	//void QueryCollision(

	
private:
	OctTreeNode m_root;
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