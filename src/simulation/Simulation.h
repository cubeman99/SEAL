#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <simulation/ObjectManager.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationConfig.h>
#include <simulation/SimulationObject.h>
#include <simulation/World.h>
#include <utilities/Random.h>


//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------
class Simulation
{
public:
	Simulation();
	~Simulation();

	void Initialize(const SimulationConfig& config);
	void Tick();
	
	inline ObjectManager* GetObjectManager() { return &m_objectManager; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return m_objectManager.GetOctTree(); }
	inline RandomNumberGenerator& GetRandom() { return m_random; }
	inline const SimulationConfig& GetConfig() { return m_config; }

	inline float GetTickRate() const { return m_tickRate; }
	inline float GetTimeStep() const { return m_timeStep; }
	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }


private:
	SimulationConfig		m_config;
	float					m_tickRate; // How many ticks per second.
	float					m_timeStep; // Length of time in seconds to simulate for each tick.
	unsigned int			m_ageInTicks;
	World					m_world;
	ObjectManager			m_objectManager;
	RandomNumberGenerator	m_random;
};


#endif // _SIMULATION_H_