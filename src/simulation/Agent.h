#ifndef _AGENT_H_
#define _AGENT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>


class Agent
{
	friend class AgentSystem;

public:
	Agent();
	~Agent();

	inline const Vector3f& GetPosition() const { return m_position; }
	inline const Quaternion& GetOrientation() const { return m_orientation; }

private:
	
	Vector3f m_position;
	Quaternion m_orientation;
	float m_moveSpeed; // Distance units per second.
	float m_turnSpeed; // Radians per second.
};


#endif // _AGENT_H_