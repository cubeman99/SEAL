#ifndef _AGENT_SYSTEM_H_
#define _AGENT_SYSTEM_H_

#include "Agent.h"
#include <vector>

class Simulation;


//-----------------------------------------------------------------------------
// AgentSystem - Manages the agents within a simulation.
//-----------------------------------------------------------------------------
class AgentSystem
{
public:
	AgentSystem(Simulation* simulation);
	~AgentSystem();

	Agent* SpawnAgent();
	void RemoveAllAgents();
	void UpdateAgents(float timeDelta);

	inline int GetNumAgents() const { return (int) m_agents.size(); }

	std::vector<Agent*>::iterator agents_begin() { return m_agents.begin(); }
	std::vector<Agent*>::iterator agents_end() { return m_agents.end(); }

private:
	void FlockAgent(Agent* agent);

	std::vector<Agent*> m_agents;
	Simulation* m_simulation;
};


#endif // _AGENT_SYSTEM_H_