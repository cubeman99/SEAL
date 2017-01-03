#include "AgentSystem.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>


AgentSystem::AgentSystem(Simulation* simulation) :
	m_simulation(simulation)
{
}

AgentSystem::~AgentSystem()
{
	RemoveAllAgents();
}

void AgentSystem::RemoveAllAgents()
{
	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
		delete *it;
	m_agents.clear();
}

Agent* AgentSystem::SpawnAgent()
{
	Agent* agent = new Agent();

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	// Randomize position (on world surface).
	agent->m_position.x = Random::NextFloat(-1, 1);
	agent->m_position.y = Random::NextFloat(-1, 1);
	agent->m_position.z = Random::NextFloat(-1, 1);
	agent->m_position.Normalize();
	agent->m_position *= worldRadius;

	// Randomize orientation (tangent to world surface).
	Vector3f axis = agent->m_position.Cross(Vector3f::UP).Normalize();
	float angle = Math::ACos(Vector3f::Normalize(agent->m_position).Dot(Vector3f::UP));
	agent->m_orientation = Quaternion(axis, angle);
	agent->m_orientation.Rotate(agent->m_orientation.GetUp(),
		Random::NextFloat() * Math::TWO_PI);

	agent->m_moveSpeed = 0.36f;
	agent->m_turnSpeed = 1.0f;

	m_agents.push_back(agent);
	return agent;
}

void AgentSystem::UpdateAgents(float timeDelta)
{
	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
	{
		Agent* agent = *it;
		
		// Turn agent.
		agent->m_orientation.Rotate(agent->m_orientation.GetUp(),
			agent->m_turnSpeed * timeDelta);

		// Move agent forward.
		Vector3f posPrev = agent->m_position;
		agent->m_position += agent->m_orientation.GetForward() * agent->m_moveSpeed * timeDelta;
		agent->m_position.Normalize();
		agent->m_position *= m_simulation->GetWorld()->GetRadius();
		float angle = Math::ACos(Vector3f::Normalize(agent->m_position).Dot(Vector3f::Normalize(posPrev)));
		agent->m_orientation.Rotate(agent->m_orientation.GetRight(), angle);
	}
}
