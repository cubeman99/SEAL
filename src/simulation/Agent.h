#ifndef _AGENT_H_
#define _AGENT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"
#include "Vision.h"

class Offshoot;


//-----------------------------------------------------------------------------
// Agent - A single being of the simulation with its own body and brain.
//-----------------------------------------------------------------------------
class Agent : public SimulationObject
{
	friend class AgentSystem;

public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::AGENT);

	Agent();
	~Agent();

	void OnSpawn() override;
	void Update(float timeDelta) override;
	void UpdateVision();
	void UpdateBrain();
	void SeeObject(SimulationObject* object);
	void EatPlant(Offshoot* plant);

	inline float GetMoveSpeed() const { return m_moveSpeed; }
	inline float GetTurnSpeed() const { return m_turnSpeed; }

	inline float GetFieldOfView() const { return m_fieldOfView; }
	inline float GetMaxViewDistance() const { return m_maxViewDistance; }
	inline float GetAngleBetweenEyes() const { return m_angleBetweenEyes; }

	inline unsigned int GetNumEyes() const { return m_numEyes; }
	inline Retina* GetEye(unsigned int index) { return &m_eyes[index]; }
	inline const Retina* GetEye(unsigned int index) const { return &m_eyes[index]; }
	
	inline void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }
	inline void SetTurnSpeed(float turnSpeed) { m_turnSpeed = turnSpeed; }
	inline void EnableWandering(bool enableWander) { m_wander = enableWander; }

private:
	
	float			m_moveSpeed; // Distance units per second.
	float			m_turnSpeed; // Radians per second.
	float			m_strength;
	float			m_energy;
	int				m_lifeSpan;
	int				m_age;

	float			m_maxMoveSpeed;
	float			m_maxTurnSpeed;
	float			m_maxEnergy;
		
	unsigned int	m_numEyes;
	Retina			m_eyes[2];
	float			m_fieldOfView; // in radians.
	float			m_maxViewDistance;
	float			m_angleBetweenEyes; // in radians.

	
	// TEMP: enable/disable temporary wandering. This is for debug
	// purposes, in case we want to control our selected agent manually.
	bool m_wander;
};


#endif // _AGENT_H_