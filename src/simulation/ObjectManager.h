#ifndef _OBJECT_MANAGER_H_
#define _OBJECT_MANAGER_H_

#include <math/Vector3f.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationObject.h>
#include <simulation/Agent.h>
#include <simulation/Plant.h>
#include <simulation/Offshoot.h>
#include <vector>

class Simulation;
class Agent;
class Plant;


//-----------------------------------------------------------------------------
// ObjectManager - Manages the objects in a simulation.
//-----------------------------------------------------------------------------
class ObjectManager
{
	template <class T_Object>
	friend class SimulationObjectIterator;

public:
	//-----------------------------------------------------------------------------
	// Tnitialization & termination

	ObjectManager(Simulation* simulation);
	~ObjectManager();

	// Initialize the object manager.
	void Initialize();

	//-----------------------------------------------------------------------------
	// Accessors

	inline Simulation* GetSimulation() { return m_simulation; }

	inline OctTree* GetOctTree() { return &m_octTree; }

	inline unsigned int GetNumObjects() const { return m_objects.size(); }

	SimulationObject* GetObj(int objectId);

	//-----------------------------------------------------------------------------
	// Object management

	// Update all objects.
	void UpdateObjects();

	// Clear (delete) all objects from the simulation.
	void ClearObjects();
	
	// Spawn an object into the simulation.
	void SpawnObject(SimulationObject* object);

	// Construct and spawn an object into the simulation at a random position.
	template <class T_Object>
	T_Object* SpawnObjectRandom();

	//-----------------------------------------------------------------------------
	// Object iteration

	inline std::vector<SimulationObject*>::iterator objects_begin() { return m_objects.begin(); }
	inline std::vector<SimulationObject*>::iterator objects_end() { return m_objects.end(); }

	SimulationObjectIterator<Agent> agents_begin();
	SimulationObjectIterator<Agent> agents_end();
	SimulationObjectIterator<Plant> plants_begin();
	SimulationObjectIterator<Plant> plants_end();

	//-----------------------------------------------------------------------------
	// Object helper functions

	// Move a simulation object forward by a given distance. This will adjust
	// its position and orientation to be aligned on the world's surface.
	void MoveObjectForward(SimulationObject* object, float distance) const;

	// Create a random position and orientation on the world's surface.
	void CreateRandomPositionAndOrientation(
		Vector3f& position, Quaternion& orientation) const;

	// Create a random position relative to another, also orientation
	void CreateRelativeRandomPositionAndOrientation( Vector3f relativePosition,
		Vector3f& newPosition, float randomFactor, Quaternion& orientation) const;

private:
	Simulation*		m_simulation;
	OctTree			m_octTree;
	int				m_objectIdCounter;
	std::vector<SimulationObject*> m_objects;
	std::map<int, SimulationObject*> m_idToObjectMap;
};


//-----------------------------------------------------------------------------
// SimulationObjectIterator - used to iterate all the objects of certain type
//                            in a simulation.
//-----------------------------------------------------------------------------
template <class T_Object>
class SimulationObjectIterator
{
public:
	typedef SimulationObjectIterator<T_Object> iterator;
	typedef T_Object*		value_type;
	typedef value_type&		reference;
	typedef value_type*		pointer;

	SimulationObjectIterator(unsigned int index, ObjectManager* objectManager) :
		m_index(index),
		m_objectManager(objectManager)
	{
		MoveToValidObject();
	}

	SimulationObjectIterator(const iterator& copy) :
		m_index(copy.m_index),
		m_objectManager(copy.m_objectManager)
	{}

	iterator& operator=(const iterator& other) { m_index = other.m_index; m_objectManager = other.m_objectManager; }
	bool operator==(const iterator& other) const { return (m_index == other.m_index); }
	bool operator!=(const iterator& other) const { return (m_index != other.m_index); }

	iterator& operator++()
	{
		++m_index;
		MoveToValidObject();
		return *this;
	}

	iterator operator++(int)
	{
		iterator clone(*this);
		++m_index;
		MoveToValidObject();
		return clone;
	}

	T_Object* operator*() const { return (T_Object*) m_objectManager->m_objects[m_index]; }
	T_Object* operator->() const { return (T_Object*) m_objectManager->m_objects[m_index]; }

private:
	void MoveToValidObject() 
	{
		while (m_index < m_objectManager->m_objects.size() &&
			(m_objectManager->m_objects[m_index]->GetObjectType() != T_Object::k_objectType ||
			m_objectManager->m_objects[m_index]->IsDestroyed()))
		{
			++m_index;
		}
	}

	unsigned int m_index;
	ObjectManager* m_objectManager;
	SimulationObjectType m_objectType;
};


//-----------------------------------------------------------------------------
// ObjectManager template method definitions
//-----------------------------------------------------------------------------

template <class T_Object>
T_Object* ObjectManager::SpawnObjectRandom()
{
	T_Object* object = new T_Object();
	
	// Randomize the object's position and orientation.
	CreateRandomPositionAndOrientation(object->m_position, object->m_orientation);
	
	SpawnObject(object);
	return object;
}



#endif // _OBJECT_MANAGER_H_