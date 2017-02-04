#ifndef _AGENT_SYSTEM_H_
#define _AGENT_SYSTEM_H_

#include "Agent.h"
#include "Plant.h"

class Simulation;

//-----------------------------------------------------------------------------
// AgentSystem - Manages the agents and plants within a simulation.
//-----------------------------------------------------------------------------
class AgentSystem
{
public:
	AgentSystem(Simulation* simulation, float plantGrowthRate, float plantMaxEnergy);
	~AgentSystem();

	Agent* SpawnAgent();
	Plant* SpawnPlant();
	void RemoveAllAgents();
	void RemoveAllPlants();
	void UpdateAgents(float timeDelta);
	void UpdatePlants(float timeDelta);
	void UpdateAgentVision(Agent* agent);

	inline int GetNumAgents() const { return (int) m_agents.size(); }
	inline int GetNumPlants() const { return (int) m_plants.size(); }

	std::vector<Agent*>::iterator agents_begin() { return m_agents.begin(); }
	std::vector<Agent*>::iterator agents_end() { return m_agents.end(); }
	std::vector<Plant*>::iterator plants_begin() { return m_plants.begin(); }
	std::vector<Plant*>::iterator plants_end() { return m_plants.end(); }

private:
	void FlockAgent(Agent* agent);

	std::vector<Agent*> m_agents;
	std::vector<Plant*> m_plants;
	Simulation* m_simulation;

	float m_plantGrowthRate;
	float m_plantMaxEnergy;
};


#endif // _AGENT_SYSTEM_H_