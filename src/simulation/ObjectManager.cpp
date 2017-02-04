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

void ObjectManager::ClearObjects()
{
	// Delete all objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];
	m_objects.clear();

	m_octTree.Clear();
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
		object->Update(timeDelta);

		m_octTree.DynamicUpdate(object);
	}
	
	// Remove any destroyed objects.
	for (unsigned int i = 0; i < m_objects.size(); ++i)
	{
		SimulationObject* object = m_objects[i];
		if (object->m_isDestroyed)
		{
			m_objects.erase(m_objects.begin() + i);
			m_octTree.RemoveObject(object);
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
	axis.Normalize();
	float angle = Math::ACos(Vector3f::Normalize(position).Dot(Vector3f::UP));
	orientation = Quaternion(axis, angle);
	orientation.Rotate(orientation.GetUp(),
		Random::NextFloat() * Math::TWO_PI);
	
	// Make sure postion is on world's surface.
	position *= m_simulation->GetWorld()->GetRadius();
}


