#include "ObjectManager.h"
#include <simulation/Simulation.h>
#include <utilities/Random.h>
#include <math/MathLib.h>


ObjectManager::ObjectManager(Simulation* simulation) :
	m_simulation(simulation)
{

}

ObjectManager::~ObjectManager()
{
	ClearObjects();
}

void ObjectManager::Initialize()
{
	m_objectIdCounter = 1;

	// Setup the OctTree.
	AABB octTreeBounds;
	Vector3f worldPos = Vector3f::ZERO;
	Vector3f worldRadius(m_simulation->GetWorld()->GetRadius() * 1.2f); // a bit larger than the world
	octTreeBounds.mins = worldPos - worldRadius;
	octTreeBounds.maxs = worldPos + worldRadius;
	m_octTree.Clear();
	m_octTree.SetBounds(octTreeBounds);
	m_octTree.SetMaxDepth(4);
	m_octTree.SetMaxObjectsPerNode(1);
}

//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

SimulationObject* ObjectManager::GetObj(int objectId)
{
	auto it = m_idToObjectMap.find(objectId);
	if (it != m_idToObjectMap.end())
		return it->second;
	return nullptr;
}


//-----------------------------------------------------------------------------
// Object management
//-----------------------------------------------------------------------------

void ObjectManager::ClearObjects()
{
	// Delete all objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];
	m_objects.clear();
	m_idToObjectMap.clear();
	m_octTree.Clear();
}

void ObjectManager::UpdateObjects()
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
		object->Update();

		// Update the object in the cct-tree since the
		// object's position probably changed.
		m_octTree.DynamicUpdate(object);
	}
	
	// Remove any destroyed objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
	{
		SimulationObject* object = m_objects[i];
		if (object->m_isDestroyed)
		{
			object->OnDestroy();
			m_objects.erase(m_objects.begin() + i);
			m_octTree.RemoveObject(object);
			m_idToObjectMap.erase(object->m_objectId);
			delete object;
			i--;
		}
	}
}

void ObjectManager::SpawnObject(SimulationObject* object)
{
	m_objects.push_back(object);
	m_idToObjectMap[m_objectIdCounter] = object;
	m_octTree.InsertObject(object);
	
	object->m_objectId = m_objectIdCounter++;
	object->m_objectManager = this;
	object->m_isDestroyed = false;
	object->OnSpawn();
}

void ObjectManager::MoveObjectForward(SimulationObject* object, float distance) const
{
	// Rotate the object's position and orientation around the 
	// center point of the world sphere.

	// Determine the axis and angle to rotate around the surface.
	Vector3f axis = object->m_orientation.GetLeft();
	float angle = distance / m_simulation->GetWorld()->GetRadius();
	Quaternion rotation(axis, angle);

	// Rotate the position and orientation.
	rotation.RotateVector(object->m_position);
	object->m_orientation.Rotate(rotation);

	//// Move the object forward linearly first.
	//Vector3f posPrev = object->m_position;
	//object->m_position += object->m_orientation.GetForward() * distance;

	//// Snap the position to the world's surface.
	//object->m_position.Normalize();
	//object->m_position *= m_simulation->GetWorld()->GetRadius();

	//// Keep the orientation tangent to the world's surface.
	//float angle = Math::ACos(Vector3f::Normalize(object->m_position).Dot(Vector3f::Normalize(posPrev)));
	//object->m_orientation.Rotate(object->m_orientation.GetRight(), angle);
}


//-----------------------------------------------------------------------------
// Object iteration
//-----------------------------------------------------------------------------

SimulationObjectIterator<Agent> ObjectManager::agents_begin()
{
	return SimulationObjectIterator<Agent>(0, this);
}

SimulationObjectIterator<Agent> ObjectManager::agents_end()
{
	return SimulationObjectIterator<Agent>(m_objects.size(), this);
}

SimulationObjectIterator<Plant> ObjectManager::plants_begin()
{
	return SimulationObjectIterator<Plant>(0, this);
}

SimulationObjectIterator<Plant> ObjectManager::plants_end()
{
	return SimulationObjectIterator<Plant>(m_objects.size(), this);
}


//-----------------------------------------------------------------------------
// Object helper functions
//-----------------------------------------------------------------------------

void ObjectManager::CreateRandomPositionAndOrientation(
	Vector3f& position, Quaternion& orientation) const
{
	RNG& random = m_simulation->GetRandom();

	// Randomize position.
	// NOTE: this is an uneven distribution for a sphere. Objects will be
	// more populated around the extremes of the coorinate axes' 8 sectors.
	position.x = random.NextFloat(-1, 1);
	position.y = random.NextFloat(-1, 1);
	position.z = random.NextFloat(-1, 1);
	position.Normalize();

	// Randomize orientation (tangent to world surface).
	Vector3f axis = Vector3f::UP.Cross(position);
	axis.Normalize();
	float angle = Math::ACos(position.Dot(Vector3f::UP));
	orientation = Quaternion(axis, angle);
	orientation.Rotate(orientation.GetUp(),
		random.NextFloat() * Math::TWO_PI);
	
	// Make sure postion is on world's surface.
	position *= m_simulation->GetWorld()->GetRadius();
}


void ObjectManager::CreateRelativeRandomPositionAndOrientation(
	Vector3f relativePosition, Vector3f& newPosition,
	float randomFactor, Quaternion& orientation) const
{
	RNG& random = m_simulation->GetRandom();

	newPosition = relativePosition;
	newPosition.x += random.NextFloatClamped() * randomFactor;
	newPosition.y += random.NextFloatClamped() * randomFactor;
	newPosition.z += random.NextFloatClamped() * randomFactor;
	newPosition.Normalize();

	// Randomize orientation (tangent to world surface).
	Vector3f axis = Vector3f::UP.Cross(newPosition);
	axis.Normalize();
	float angle = Math::ACos(newPosition.Dot(Vector3f::UP));
	orientation = Quaternion(axis, angle);
	orientation.Rotate(orientation.GetUp(),
		random.NextFloat() * Math::TWO_PI);

	// Make sure postion is on world's surface.
	newPosition *= m_simulation->GetWorld()->GetRadius();
}

