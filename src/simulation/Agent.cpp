#include "Agent.h"
#include <utilities/Random.h>
#include <simulation/ObjectManager.h>
#include <math/MathLib.h>
#include <math/Vector2f.h>


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
	
	m_moveSpeed = 0.1f;
	m_turnSpeed = 1.0f;
}

Agent::~Agent()
{
}

void Agent::Update(float timeDelta)
{
	UpdateVision();
	UpdateBrain();

	// Turn and move.
	m_orientation.Rotate(m_orientation.GetUp(), m_turnSpeed * timeDelta);
	m_objectManager->MoveObjectForward(this, m_moveSpeed * timeDelta);
}

void Agent::UpdateVision()
{
	// Clear sight.
	m_eyes[0].ClearSightValues();
	m_eyes[1].ClearSightValues();

	// Attempt to see each object.
	for (auto it = m_objectManager->objects_begin(); it != m_objectManager->objects_end(); ++it)
	{
		SimulationObject* object = *it;
		if (object == this || !object->IsVisible())
			continue;
		SeeObject(object);
	}
}

void Agent::SeeObject(SimulationObject* object)
{
	Vector3f vecToObj = object->GetPosition() - m_position;
	float distSqr = vecToObj.LengthSquared();
	if (distSqr > m_maxViewDistance * m_maxViewDistance)
		return;

	// Get the distance to the object with respect to its forward and right vectors.
	Vector3f forward = m_orientation.GetForward();
	Vector3f right = m_orientation.GetRight();
	Vector2f p(vecToObj.Dot(right), vecToObj.Dot(forward));
	p.Normalize();

	// Calculate the angle offset from the agent's forward vector.
	float angleOffset = Math::ATan2(p.x, p.y);// * Math::Sign(p.x);
	//if (Math::Abs(angleOffset) > m_fieldOfView)
		//continue;

	float fov = GetFieldOfView();
	float t1 = (angleOffset - GetAngleBetweenEyes() * 0.5f) / fov;
	float t2 = (angleOffset + (GetAngleBetweenEyes() * 0.5f) + fov) / fov;

	Vector3f objColor = object->GetColor();

	if (t1 >= 0.0f && t1 <= 1.0f)
	{
		Retina* eye = &m_eyes[0];
		for (unsigned int channel = 0; channel < eye->GetNumChannels(); channel++)
		{
			unsigned int index = (unsigned int) (t1 * eye->GetResolution(channel));
			index = Math::Min(index, eye->GetResolution(channel) - 1);
			eye->SetSightValue(channel, index, objColor[channel]);
		}
	}
	if (t2 >= 0.0f && t2 <= 1.0f)
	{
		Retina* eye = &m_eyes[1];
		for (unsigned int channel = 0; channel < eye->GetNumChannels(); channel++)
		{
			unsigned int index = (unsigned int) (t2 * eye->GetResolution(channel));
			index = Math::Min(index, eye->GetResolution(channel) - 1);
			eye->SetSightValue(channel, index, objColor[channel]);
		}
	}
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
