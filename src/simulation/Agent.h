#ifndef _AGENT_H_
#define _AGENT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"


//-----------------------------------------------------------------------------
// Agent - A single being of the simulation with its own body and brain.
//-----------------------------------------------------------------------------
class Agent : public SimulationObject
{
	friend class AgentSystem;

public:
	Agent();
	~Agent();

	void UpdateBrain();

	inline float GetMoveSpeed() const { return m_moveSpeed; }
	inline float GetTurnSpeed() const { return m_turnSpeed; }

private:
	
	float m_moveSpeed; // Distance units per second.
	float m_turnSpeed; // Radians per second.
};


#endif // _AGENT_H_