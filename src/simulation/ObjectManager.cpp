#include "ObjectManager.h"
#include <simulation/Simulation.h>
#include <utilities/Random.h>
#include <math/MathLib.h>


ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{
	ClearObjects();
}

void ObjectManager::ClearObjects()
{
	// Delete all objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];
	m_objects.clear();

	m_octree.Clear();
}
	
template <class T_Object>
T_Object* ObjectManager::SpawnObject(const Vector3f& position, const Vector3f& forward)
{
	T_Object* object = new T_Object();
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
	m_octree.InsertObject(object);
}

void ObjectManager::UpdateObjects(float timeDelta)
{
	// Update all objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
	{
		SimulationObject* object = m_objects[i];
		if (object->m_isDestroyed)
			continue;

		// Calculate derived data.
		object->m_worldToObject = 
			Matrix4f::CreateRotation(object->m_orientation.GetConjugate()) *
			Matrix4f::CreateTranslation(-object->m_position);
		object->m_objectToWorld = 
			Matrix4f::CreateTranslation(object->m_position) *
			Matrix4f::CreateRotation(object->m_orientation);

		// Update the object.
		//object->Update(timeDelta);

		m_simulation->GetOctTree()->DynamicUpdate(object);
	}
	
	// Remove any destroyed objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
	{
		SimulationObject* object = m_objects[i];
		if (object->m_isDestroyed)
		{
			m_objects.erase(m_objects.begin() + i);
			m_octree.RemoveObject(object);
			delete object;
			i--;
		}
	}
}

void ObjectManager::MoveObjectForward(SimulationObject* object, float distance) const
{
	// Determine the axis and angle to rotate around the surface.
	Vector3f axis = object->m_orientation.GetRight();
	float angle = distance * m_simulation->GetWorld()->GetRadius();
	Quaternion rotation(axis, angle);

	// Rotate the position AND orientation.
	rotation.RotateVector(object->m_position);
	object->m_orientation.Rotate(rotation);

	// Move the object forward linearly first.
	//Vector3f posPrev = object->m_position;
	//object->m_position += object->m_orientation.GetForward() * distance;

	// Snap the position to the world's surface.
	//object->m_position.Normalize();
	//object->m_position *= m_simulation->GetWorld()->GetRadius();

	// Keep the orientation tangent to the world's surface.
	//float angle = Math::ACos(Vector3f::Normalize(object->m_position).Dot(Vector3f::Normalize(posPrev)));
	//object->m_orientation.Rotate(object->m_orientation.GetRight(), angle);
}

void ObjectManager::CreateRandomPositionAndOrientation(
	Vector3f& position, Quaternion& orientation) const
{
	// Randomize position.
	// NOTE: this is an uneven distribution for a sphere. Objects will be
	// more populated around the extremes of the coorinate axes' 8 sectors.
	position.x = Random::NextFloat(-1, 1);
	position.y = Random::NextFloat(-1, 1);
	position.z = Random::NextFloat(-1, 1);
	position.Normalize();

	// Randomize orientation (tangent to world surface).
	Vector3f axis = position.Cross(Vector3f::UP);
	float angle = Math::ACos(Vector3f::Normalize(position).Dot(Vector3f::UP));
	orientation = Quaternion(axis, angle);
	orientation.Rotate(orientation.GetUp(),
		Random::NextFloat() * Math::TWO_PI);
	
	// Make sure postion is on world's surface.
	position *= m_simulation->GetWorld()->GetRadius();
}


