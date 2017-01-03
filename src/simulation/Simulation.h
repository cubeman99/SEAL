#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "World.h"
#include "AgentSystem.h"


class Simulation
{
public:
	Simulation();

	void Tick();
	
	AgentSystem* GetAgentSystem() { return &m_agentSystem; }
	World* GetWorld() { return &m_world; }

	inline float GetTickRate() const { return m_tickRate; }
	inline float GetTimeStep() const { return m_timeStep; }

private:
	float m_tickRate; // How many ticks per second.
	float m_timeStep; // Length of time in seconds to simulate for each tick.

	World m_world;
	AgentSystem m_agentSystem;
};


#endif // _SIMULATION_H_