#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "World.h"
#include "AgentSystem.h"
#include "PlantSystem.h"
#include "OctTree.h"
#include "SimulationObject.h"



//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------
class Simulation
{
public:
	Simulation();

	void Initialize();
	void Tick();
	
	inline AgentSystem* GetAgentSystem() { return &m_agentSystem; }
	inline PlantSystem* GetPlantSystem() { return &m_plantSystem; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return &m_octTree; }

	inline float GetTickRate() const { return m_tickRate; }
	inline float GetTimeStep() const { return m_timeStep; }
	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }


private:
	float			m_tickRate; // How many ticks per second.
	float			m_timeStep; // Length of time in seconds to simulate for each tick.

	unsigned int	m_ageInTicks;
	World			m_world;
	AgentSystem		m_agentSystem;
	PlantSystem		m_plantSystem;
	OctTree			m_octTree;
};


#endif // _SIMULATION_H_