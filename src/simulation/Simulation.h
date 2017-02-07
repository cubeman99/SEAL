#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <simulation/World.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationObject.h>
#include <simulation/ObjectManager.h>
#include <utilities/Random.h>

//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------
class Simulation
{
public:
	Simulation();

	void Initialize();
	void Tick();
	
	//inline AgentSystem* GetAgentSystem() { return &m_agentSystem; }
	inline ObjectManager* GetObjectManager() { return &m_objectManager; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return m_objectManager.GetOctTree(); }// &m_octTree; }

	inline float GetTickRate() const { return m_tickRate; }
	inline float GetTimeStep() const { return m_timeStep; }
	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }

	inline RandomNumberGenerator& GetRandom() { return m_random; }

private:
	float			m_tickRate; // How many ticks per second.
	float			m_timeStep; // Length of time in seconds to simulate for each tick.

	unsigned int	m_ageInTicks;
	World			m_world;
	//AgentSystem		m_agentSystem;
	//OctTree			m_octTree;

	ObjectManager	m_objectManager;

	RandomNumberGenerator	m_random;
};


#endif // _SIMULATION_H_