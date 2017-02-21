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

SimulationObject* ObjectManager::GetObjectById(int objectId)
{
	auto it = m_idToObjectMap.find(objectId);
	if (it != m_idToObjectMap.end())
		return it->second;
	return nullptr;
}

SimulationObject* ObjectManager::GetObjByIndex(unsigned int index)
{
	if (index < GetNumObjects())
	{
		return m_objects[index];
	}
	
	return nullptr;
}


//-----------------------------------------------------------------------------
// Object management
//-----------------------------------------------------------------------------

void ObjectManager::ClearObjects()
{
	m_idToObjectMap.clear();
	m_octTree.Clear();
	// Delete all objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];
	m_objects.clear();
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
	
	object->m_objectId = m_objectIdCounter;	// TODO: since this sim is meant to run for a long time, make sure
	m_objectIdCounter += 1;

	object->m_objectManager = this;				// this doesn't exceed INT_MAX
	object->m_isDestroyed = false;
	object->OnSpawn();

}

bool ObjectManager::SpawnObjectSerialized(std::ifstream& fileIn)
{
	SimulationObjectType objectType;
	SimulationObject* nextObject;

	fileIn.read((char*)&objectType, sizeof(SimulationObjectType));

	switch (objectType)
	{
	case AGENT:
		nextObject = new Agent();
		break;

	case PLANT:
		nextObject = new Plant();
		break;

	case OFFSHOOT:
		nextObject = new Offshoot();
		break;

	case CARCASS:
		// TODO: when carcass implemented
		//break;

	default:
		// Object type not found
		// Data is missing or corrupt
		return false;
		break;
	}

	// Read in data
	nextObject->m_isDestroyed = false;
	nextObject->m_objectManager = this;
	nextObject->Read(fileIn);

	// Incorporate
	m_objects.push_back(nextObject);
	m_idToObjectMap[nextObject->GetId()] = nextObject;
	m_octTree.InsertObject(nextObject);

	// On spawn
	nextObject->OnSpawn();

	// Set Id counter
	// Once the last object has been read, the Id counter should match that of the loaded timelime
	m_objectIdCounter = nextObject->GetId() + 1;

	nextObject = nullptr;
	return true;
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
	
	// Randomize position (this should be a uniform distribution for a sphere).
	float pitchAngle = Math::ASin(random.NextFloat(-1.0f, 1.0f));
	float yawAngle = random.NextFloat(0.0f, Math::TWO_PI);
	position = Vector3f::NEG_UNITZ;
	position.Rotate(Vector3f::UNITX, pitchAngle);
	position.Rotate(Vector3f::UNITY, yawAngle);

	// Randomize orientation (tangent to world surface).
	CreateRandomOrientation(position, orientation);
	
	// Make sure postion is on world's surface.
	position *= m_simulation->GetWorld()->GetRadius();
}

// Create a random orientation on the world's surface.
void ObjectManager::CreateRandomOrientation(
	const Vector3f& position, Quaternion& orientation) const
{
	RNG& random = m_simulation->GetRandom();

	// Randomize orientation (tangent to world surface).
	Vector3f axis = Vector3f::UP.Cross(position);
	axis.Normalize();
	float angle = Math::ACos(position.Dot(Vector3f::UP));
	orientation = Quaternion(axis, angle);
	orientation.Rotate(orientation.GetUp(),
		random.NextFloat() * Math::TWO_PI);
	
}

void ObjectManager::CreateRelativeRandomPositionAndOrientation(
	Vector3f relativePosition, Vector3f& newPosition,
	float randomFactor, Quaternion& orientation) const
{
	RNG& random = m_simulation->GetRandom();

	// Find the tangent and bitangent axes.
	Vector3f tangent;
	if (Math::Abs(relativePosition.x) < Math::Abs(relativePosition.y))
		tangent = relativePosition.Cross(Vector3f::UNITX);
	else
		tangent = relativePosition.Cross(Vector3f::UNITY);
	tangent.Normalize();
	Vector3f bitangent = tangent.Cross(relativePosition);
	bitangent.Normalize();

	// Randomly offset the position in a circular area around the
	// relative position that's tangent to the world surface.
	// Modified from: http://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
    float theta = Math::TWO_PI * random.NextFloat();
    float u = random.NextFloat() + random.NextFloat();
	float radius = (u > 1 ? 2 - u : u) * randomFactor;
	newPosition = relativePosition;
	newPosition += tangent * radius * Math::Cos(theta);
	newPosition += bitangent * radius * Math::Sin(theta);
	newPosition.Normalize();

	// Randomize orientation (tangent to world surface).
	CreateRandomOrientation(newPosition, orientation);

	// Make sure postion is on world's surface.
	newPosition *= m_simulation->GetWorld()->GetRadius();
}

