#ifndef _SIMULATION_OBJECT_H_
#define _SIMULATION_OBJECT_H_

#include <math/Sphere.h>
#include <math/AABB.h>
#include <math/Matrix4f.h>
#include <math/Quaternion.h>

class ObjectManager;
class Simulation;


//-----------------------------------------------------------------------------
// SimulationObjectType - Defines the types of simulation objects.
//-----------------------------------------------------------------------------

// TODO: start using this enum somehow
enum SimulationObjectType : int
{
	INVALID = '?',

	AGENT = 'AGNT',
	PLANT = 'PLNT',
	OFFSHOOT = 'OFST',
	//CARCASS = 'CARC',
	//PREDATOR_AGENT = 'PRED',
	//PREY_AGENT = 'PREY',
	//FOOD,
};

// Use this in a subclass of SimulationObject to declare its type.
#define DECLARE_SIMULATION_OBJECT(_type) \
	enum { k_objectType = _type }; \
	virtual SimulationObjectType GetObjectType() const override { return _type; }


//-----------------------------------------------------------------------------
// SimulationObject - The base class for all objects in the simulation.
//-----------------------------------------------------------------------------
class SimulationObject
{
	friend class ObjectManager;

public:
	SimulationObject();
	virtual ~SimulationObject() {}
	
	// Virtual object methods

	virtual void OnSpawn() {}
	virtual void OnDestroy() {}
	virtual void Update(float timeDelta) {}

	// Object type

	virtual SimulationObjectType GetObjectType() const { return SimulationObjectType::INVALID; }
	template <class T_Object>
	bool IsObjectType() const { return (GetObjectType() == T_Object::k_objectType); }

	// Object operations

	void Destroy();

	// Getters
	
	ObjectManager* GetObjectManager();
	Simulation* GetSimulation();
	Sphere GetBoundingSphere();
	AABB GetBoundingBox();
	inline const Vector3f& GetPosition() const { return m_position; }
	inline const Quaternion& GetOrientation() const { return m_orientation; }
	inline const Vector3f& GetColor() const { return m_color; }
	inline float GetRadius() const { return m_radius; }
	inline bool IsDestroyed() const { return m_isDestroyed; }
	inline bool IsVisible() const { return m_isVisible; }
	inline const Matrix4f& GetObjectToWorld() const { return m_objectToWorld; }
	inline const Matrix4f& GetWorldToObject() const { return m_worldToObject; }

	// Setters

	inline void SetPosition(const Vector3f& position) { m_position = position; }
	inline void SetOrientation(const Quaternion& orientation) { m_orientation = orientation; }

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