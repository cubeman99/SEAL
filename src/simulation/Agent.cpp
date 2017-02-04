#include "Agent.h"
#include <utilities/Random.h>


Agent::Agent()
{
	m_fieldOfView = 0.9f;
	m_maxViewDistance = 0.2f;
	m_angleBetweenEyes = 0.5f;
	m_resolutions[0] = 14;
	m_resolutions[1] = 8;
	m_resolutions[2] = 2;

	m_numEyes = 2;
	unsigned int resolutions[3];
	resolutions[0] = 8;
	resolutions[1] = 13;
	resolutions[2] = 2;
	m_eyes[0].Configure(m_fieldOfView, m_maxViewDistance, 3, m_resolutions);
	m_eyes[1].Configure(m_fieldOfView, m_maxViewDistance, 3, m_resolutions);
	
	m_color.Set(0, 0, 1.0f);
	m_color.x = Random::NextFloat();
	m_color.y = Random::NextFloat();
	m_color.z = Random::NextFloat();
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
