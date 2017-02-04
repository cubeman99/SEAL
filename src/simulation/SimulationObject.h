#ifndef _SIMULATION_OBJECT_H_
#define _SIMULATION_OBJECT_H_

#include <math/Sphere.h>
#include <math/AABB.h>
#include <math/Quaternion.h>


enum SimulationObjectType
{
	//AGENT,
	//	PREDATOR_AGENT,
	//	PREY_AGENT,
	//FOOD,
	//	PLANT,
	//	CARCASS,
	//PLANT_SOURCE,
};


//-----------------------------------------------------------------------------
// SimulationObject - The base class for all objects in the simulation.
//-----------------------------------------------------------------------------
class SimulationObject
{
public:
	SimulationObject();
	
	// Getters
	inline const Vector3f& GetPosition() const { return m_position; }
	inline const Quaternion& GetOrientation() const { return m_orientation; }
	inline const Vector3f& GetColor() const { return m_color; }
	inline float GetRadius() const { return m_radius; }
	Sphere GetBoundingSphere();
	AABB GetBoundingBox();



protected:
	Vector3f	m_color;
	Vector3f	m_position;
	Quaternion	m_orientation;
	float		m_radius;
};


#endif // _SIMULATION_OBJECT_H_