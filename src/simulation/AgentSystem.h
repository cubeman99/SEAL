#ifndef _AGENT_SYSTEM_H_
#define _AGENT_SYSTEM_H_

#include "Agent.h"
#include <vector>

class Simulation;


class AgentSystem
{
public:
	AgentSystem(Simulation* simulation);
	~AgentSystem();

	Agent* SpawnAgent();
	void RemoveAllAgents();
	void UpdateAgents(float timeDelta);

	std::vector<Agent*>::iterator agents_begin() { return m_agents.begin(); }
	std::vector<Agent*>::iterator agents_end() { return m_agents.end(); }

private:
	std::vector<Agent*> m_agents;
	Simulation* m_simulation;
};


#endif // _AGENT_SYSTEM_H_