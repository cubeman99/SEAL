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
	ObjectManager(Simulation* simulation);
	~ObjectManager();

	void Initialize();
	void UpdateObjects(float timeDelta);
	void ClearObjects();
	
	template <class T_Object>
	T_Object* SpawnObject(const Vector3f& position, const Vector3f& forward);

	template <class T_Object>
	T_Object* SpawnObjectRandom();
	
	std::vector<SimulationObject*>::iterator objects_begin() { return m_objects.begin(); }
	std::vector<SimulationObject*>::iterator objects_end() { return m_objects.end(); }
	
	void MoveObjectForward(SimulationObject* object, float distance) const;
	void CreateRandomPositionAndOrientation(Vector3f& position, Quaternion& orientation) const;
	//void CreateRandomOrientation(Vector3f& position, Quaternion& orientation) const;


private:
	Simulation*		m_simulation;
	OctTree			m_octTree;
	unsigned int	m_objectIdCounter;
	std::vector<SimulationObject*> m_objects;
};

	
template <class T_Object>
T_Object* ObjectManager::SpawnObject(const Vector3f& position, const Vector3f& forward)
{
	T_Object* object = new T_Object();
	object->m_objectManager = this;
	object->m_isDestroyed = false;
	
	// Orthoganalize the orientation to be tangent to the world's surface.
	Vector3f basisUp = position;
	basisUp.Normalize();
	Vector3f basisRight = forward.Cross(basisUp);
	basisRight.Normalize();
	Vector3f basisForward = basisUp.Cross(basisRight);
	//Quaternion orientation = Quaternion::FromMatrix(;
	//object->SetOrientation(orientation);

	// Make sure postion is on world's surface.
	object->SetPosition(basisUp * m_simulation->GetWorld()->GetRadius());

	// Insert the object.
	m_objects.push_back(object);
	m_octTree.InsertObject(object);
	return object;
}
	
template <class T_Object>
T_Object* ObjectManager::SpawnObjectRandom()
{
	T_Object* object = new T_Object();
	object->m_objectManager = this;
	object->m_isDestroyed = false;
	
	// Randomize the object's position and orientation.
	CreateRandomPositionAndOrientation(object->m_position, object->m_orientation);

	// Insert the object.
	m_objects.push_back(object);
	m_octTree.InsertObject(object);
	return object;
}


#endif // _OBJECT_MANAGER_H_