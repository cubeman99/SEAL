#include "Agent.h"
#include <utilities/Random.h>


Agent::Agent()
{
	m_fieldOfView = 0.9f;
	m_maxViewDistance = 0.2f;
	m_angleBetweenEyes = 0.5f;
	m_resolutions[0] = 8;
	m_resolutions[1] = 8;
	m_resolutions[2] = 8;
}

Agent::~Agent()
{
}

void Agent::UpdateBrain()
{
	// TEMP: random wandering turning.

	
	//float timeDelta = 0.01666667f;
	float maxTurnSpeed = 6.0f;
	
	float chance = ((Random::NextFloat() * 2) - 1) * maxTurnSpeed;
	float acc = 1;
	if (chance <= m_turnSpeed)
		acc = -1;
	m_turnSpeed += Random::NextFloat() * acc * maxTurnSpeed * 0.1f;
	
}
