#ifndef _SIMULATION_OBJECT_H_
#define _SIMULATION_OBJECT_H_

#include <math/Sphere.h>
#include <math/AABB.h>
#include <math/Matrix4f.h>
#include <math/Quaternion.h>

class ObjectManager;


// TODO: start using this enum somehow
enum SimulationObjectType : unsigned int
{
	AGENT = 'AGNT',
	PREDATOR_AGENT = 'PRED',
	PREY_AGENT = 'PREY',
	FOOD,
	PLANT = 'PLNT',
	CARCASS = 'CARC',
	PLANT_SOURCE = 'PSRC',
};

#define DECLARE_SIMULATION_OBJECT(_type) \
	enum { k_objectType = _type };


//-----------------------------------------------------------------------------
// SimulationObject - The base class for all objects in the simulation.
//-----------------------------------------------------------------------------
class SimulationObject
{
	friend class ObjectManager;

public:
	SimulationObject();
	
	// Getters
	inline void SetPosition(const Vector3f& position) { m_position = position; }
	inline void SetOrientation(const Quaternion& orientation) { m_orientation = orientation; }

	inline const Vector3f& GetPosition() const { return m_position; }
	inline const Quaternion& GetOrientation() const { return m_orientation; }
	inline const Vector3f& GetColor() const { return m_color; }
	inline float GetRadius() const { return m_radius; }
	inline bool IsVisible() const { return m_isVisible; }
	Sphere GetBoundingSphere();
	AABB GetBoundingBox();



protected:
	bool		m_isVisible;
	Vector3f	m_color;
	Vector3f	m_position;
	Quaternion	m_orientation;
	float		m_radius;

	bool		m_isDestroyed;

	ObjectManager* m_objectManager;

protected:
	// Derived data.
	Matrix4f	m_objectToWorld;
	Matrix4f	m_worldToObject;
};


#endif // _SIMULATION_OBJECT_H_