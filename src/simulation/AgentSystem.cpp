#include "AgentSystem.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>


AgentSystem::AgentSystem(Simulation* simulation, float plantGrowthRate, float plantMaxEnergy) :
	m_simulation(simulation),
	m_plantGrowthRate(plantGrowthRate),
	m_plantMaxEnergy(plantMaxEnergy)
{
}

AgentSystem::~AgentSystem()
{
	RemoveAllAgents();
	RemoveAllPlants();
}

void AgentSystem::RemoveAllAgents()
{
	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
		delete *it;
	m_agents.clear();
}

void AgentSystem::RemoveAllPlants()
{
	for (auto it = m_plants.begin(); it != m_plants.end(); ++it)
		delete *it;
	m_plants.clear();
}

Agent* AgentSystem::SpawnAgent()
{
	Agent* agent = new Agent();

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	// Randomize position (on world surface).
	// NOTE: this is an uneven random distribution.
	// Objects will be more populated around the extremes of the coorinate axes' 8 sectors.
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

	agent->m_moveSpeed = 0.1f;
	agent->m_turnSpeed = 1.0f;

	m_agents.push_back(agent);

	// Insert the agent into the oct-tree.
	m_simulation->GetOctTree()->InsertObject(agent);

	return agent;
}

Plant* AgentSystem::SpawnPlant()
{
	Plant* plant = new Plant(m_simulation, m_plantMaxEnergy);

	float worldRadius = m_simulation->GetWorld()->GetRadius();

	// Randomize position (on world surface).
	plant->m_position.x = Random::NextFloat(-1, 1);
	plant->m_position.y = Random::NextFloat(-1, 1);
	plant->m_position.z = Random::NextFloat(-1, 1);
	plant->m_position.Normalize();
	plant->m_position *= worldRadius;

	m_plants.push_back(plant);

	// Insert the agent into the oct-tree.		// TODO: make this not crash
	m_simulation->GetOctTree()->InsertObject(plant);

	return plant;
}

void AgentSystem::UpdateAgents(float timeDelta)
{
	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
	{
		Agent* agent = *it;

		UpdateAgentVision(agent);
	
		agent->UpdateBrain();

		// Turn the agent's orientation.
		agent->m_orientation.Rotate(agent->m_orientation.GetUp(),
			agent->m_turnSpeed * timeDelta);

		//FlockAgent(agent);

		// Move the agent forward linearly first.
		Vector3f posPrev = agent->m_position;
		agent->m_position += agent->m_orientation.GetForward() * agent->m_moveSpeed * timeDelta;

		// Snap the position to the world's surface.
		agent->m_position.Normalize();
		agent->m_position *= m_simulation->GetWorld()->GetRadius();

		// Keep the orientation tangent to the world's surface.
		float angle = Math::ACos(Vector3f::Normalize(agent->m_position).Dot(Vector3f::Normalize(posPrev)));
		agent->m_orientation.Rotate(agent->m_orientation.GetRight(), angle);
		
		m_simulation->GetOctTree()->DynamicUpdate(agent);
	}
}

void AgentSystem::UpdatePlants(float timeDelta)
{
	bool usedUp;

	for (auto it = m_plants.begin(); it != m_plants.end(); ++it)
	{
		Plant* plant = *it;

		usedUp = plant->Update(timeDelta * m_plantGrowthRate);

		if (usedUp)
		{
			float worldRadius = m_simulation->GetWorld()->GetRadius();

			// Randomize position (on world surface).
			plant->m_position.x = Random::NextFloat(-1, 1);
			plant->m_position.y = Random::NextFloat(-1, 1);
			plant->m_position.z = Random::NextFloat(-1, 1);
			plant->m_position.Normalize();
			plant->m_position *= worldRadius;

			m_simulation->GetOctTree()->DynamicUpdate(plant);

			// Regrow some offshoots
			plant->Init();
		}
	}
}

void AgentSystem::UpdateAgentVision(Agent* agent)
{
	agent->GetEye(0)->ClearSightValues();
	agent->GetEye(1)->ClearSightValues();

	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
	{
		Agent* obj = *it;
		if (obj == agent)
			continue;
		SeeObject(agent, obj);
	}
	
	for (auto it = m_plants.begin(); it != m_plants.end(); ++it)
	{
		Plant* plant = *it;
		for (auto it2 = plant->m_offshoots.begin(); it2 != plant->offshoots_end(); ++it2)
			SeeObject(agent, *it2);
	}
}

void AgentSystem::SeeObject(Agent* agent, SimulationObject* object)
{
	Vector3f vecToObj = object->GetPosition() - agent->m_position;
	float distSqr = vecToObj.LengthSquared();
	if (distSqr > agent->m_maxViewDistance * agent->m_maxViewDistance)
		return;

	// Get the distance to the object with respect to its forward and right vectors.
	Vector3f forward = agent->m_orientation.GetForward();
	Vector3f right = agent->m_orientation.GetRight();
	Vector2f p(vecToObj.Dot(right), vecToObj.Dot(forward));
	p.Normalize();

	// Calculate the angle offset from the agent's forward vector.
	float angleOffset = Math::ATan2(p.x, p.y);// * Math::Sign(p.x);
	//if (Math::Abs(angleOffset) > agent->m_fieldOfView)
		//continue;

	float fov = agent->GetFieldOfView();
	float t1 = (angleOffset - agent->GetAngleBetweenEyes() * 0.5f) / fov;
	float t2 = (angleOffset + (agent->GetAngleBetweenEyes() * 0.5f) + fov) / fov;

	Vector3f objColor = object->GetColor();

	if (t1 >= 0.0f && t1 <= 1.0f)
	{
		Retina* eye = agent->GetEye(0);
		for (unsigned int channel = 0; channel < eye->GetNumChannels(); channel++)
		{
			unsigned int index = (unsigned int) (t1 * eye->GetResolution(channel));
			index = Math::Min(index, eye->GetResolution(channel) - 1);
			eye->SetSightValue(channel, index, objColor[channel]);
		}
	}
	if (t2 >= 0.0f && t2 <= 1.0f)
	{
		Retina* eye = agent->GetEye(1);
		for (unsigned int channel = 0; channel < eye->GetNumChannels(); channel++)
		{
			unsigned int index = (unsigned int) (t2 * eye->GetResolution(channel));
			index = Math::Min(index, eye->GetResolution(channel) - 1);
			eye->SetSightValue(channel, index, objColor[channel]);
		}
	}
}

void AgentSystem::FlockAgent(Agent* agent)
{
	Vector2f alignment = Vector2f::ZERO;
	Vector2f cohesion = Vector2f::ZERO;
	Vector2f separation = Vector2f::ZERO;
	
	Vector3f myDir = agent->GetOrientation().GetForward();
	Vector3f myRight = agent->GetOrientation().GetRight();
	Vector3f myForward = agent->GetOrientation().GetForward();
	Vector3f myPos = agent->GetPosition();

	int neighborCount = 0;
	float cohesionAvgDist = 0.0f;

	//float radius = m_simulation->GetWorld()->GetRadius() * 0.3f; 
	//float radius = 0.016f * 3; 
	float radius = 0.15f;
	float maxRadius = 0.2f;
	float minRadius = 0.1f;

	for (auto it = m_agents.begin(); it != m_agents.end(); ++it)
	{
		Agent* other = *it;
		if (agent != other)
		{
			Vector3f otherDir = other->GetOrientation().GetForward();
			Vector3f otherPos = other->GetPosition();
			
			// Rotate the velocity into our space.
			Vector2f vel(otherDir.Dot(myRight), otherDir.Dot(myForward));
			Vector2f pos((otherPos - myPos).Dot(myRight), (otherPos - myPos).Dot(myForward));
			float dist = pos.Length();

			if (dist < maxRadius)// && dist > minRadius)
			{

				alignment += vel;
				//if (dist > radius)
					//cohesion += Vector2f::Normalize(pos) * (1 - Math::Cos((Math::TWO_PI * (dist - radius)) / (maxRadius - radius))) * 0.5f;
				//else
					//cohesion -= Vector2f::Normalize(pos) * ((radius - dist) / (radius - minRadius)) * 2;
					//cohesion -= Vector2f::Normalize(pos) * (1 - Math::Cos((Math::TWO_PI * (radius - dist)) / (radius))) * 0.5f;
				//cohesion += Vector2f::Normalize(pos) * ((dist - radius) / (maxRadius - radius));
				cohesion += Vector2f::Normalize(pos) * (dist - radius) / (radius);
				separation += Vector2f::Normalize(pos) / dist;
				//cohesionAvgDist += (pos.Length() / 0.3f);

				neighborCount++;
			}
		}
	}

	if (neighborCount == 0)
		return;
	float invNeighborCount = 1.0f / (float) neighborCount;



	cohesionAvgDist *= invNeighborCount;

	alignment *= invNeighborCount;
	alignment.Normalize();
	//cohesion *= invNeighborCount;
	//cohesion *= 0.1f;
	cohesion.Normalize();
	separation *= -invNeighborCount;
	separation.Normalize();

	separation = Vector2f(
		Random::NextFloatClamped() * 0.1f,
		Random::NextFloatClamped() * 0.1f);
	separation.Normalize();

	float alignmentWeight = 1.0f;
	float cohesionWeight = 1.0f;// / cohesionAvgDist;
	float separationWeight = 0.0f;// * cohesionAvgDist;
	separationWeight += neighborCount / 40.0f;
	float weightSum = alignmentWeight + cohesionWeight + separationWeight;

	Vector2f velocity = (alignment * alignmentWeight) +
		(cohesion * cohesionWeight) +
		(separation * separationWeight);
	velocity /= weightSum;
	velocity.Normalize();

	/*velocity += Vector2f(
		Random::NextFloatClamped() * 0.1f,
		Random::NextFloatClamped() * 0.1f);
	velocity.Normalize();*/

	//Vector3f axis = Vector3f(0.0f, 1.0f, 0.0f).Cross(Vector3f(velocity.x, velocity.y, 0.0f));
	float angle = Math::ACos(velocity.Dot(Vector2f(0.0f, 1.0f)));
	if (velocity.x < 0)
		angle *= -1.0f;
	angle *= 0.1f;
	Quaternion orientation = agent->GetOrientation();
	orientation.Rotate(orientation.GetUp(), angle);

	agent->m_orientation = orientation;

	//agent->m_orientation = Quaternion::Lerp(agent->m_orientation, orientation, 0.5f, true);
	agent->m_turnSpeed = 0.0f;
	agent->m_moveSpeed = 0.2f;// / (1 + Math::Abs(angle) * 2);
}
