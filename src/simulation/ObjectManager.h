#ifndef _OBJECT_MANAGER_H_
#define _OBJECT_MANAGER_H_

#include <math/Vector3f.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationObject.h>
#include <vector>

class Simulation;


//-----------------------------------------------------------------------------
// ObjectManager - Manages the objects in a simulation.
//-----------------------------------------------------------------------------
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void UpdateObjects(float timeDelta);
	void ClearObjects();
	
	template <class T_Object>
	T_Object* SpawnObject(const Vector3f& position, const Vector3f& forward);
	
	std::vector<SimulationObject*>::iterator objects_begin() { return m_objects.begin(); }
	std::vector<SimulationObject*>::iterator objects_end() { return m_objects.end(); }
	
	void MoveObjectForward(SimulationObject* object, float distance) const;
	void CreateRandomPositionAndOrientation(Vector3f& position, Quaternion& orientation) const;
	//void CreateRandomOrientation(Vector3f& position, Quaternion& orientation) const;


private:
	Simulation*		m_simulation;
	OctTree			m_octree;
	unsigned int	m_objectIdCounter;
	std::vector<SimulationObject*> m_objects;
};


#endif // _OBJECT_MANAGER_H_