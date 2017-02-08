#include "Agent.h"
#include <utilities/Random.h>
#include <simulation/ObjectManager.h>
#include <simulation/Simulation.h>
#include <math/MathLib.h>
#include <math/Vector2f.h>


Agent::Agent() :
	m_moveSpeed(0.0f),
	m_turnSpeed(0.0f),
	m_numEyes(2)
{
}

Agent::~Agent()
{
}

void Agent::OnSpawn()
{
	m_energy	= 100.0f;
	m_maxEnergy	= 100.0f;
	
	// Randomize vision parameters.
	float worldRadius = GetSimulation()->GetWorld()->GetRadius();
	m_angleBetweenEyes = Random::NextFloat(0, 90) * Math::DEG_TO_RAD;
	m_fieldOfView = Random::NextFloat(20, 150) * Math::DEG_TO_RAD;
	m_maxViewDistance = Random::NextFloat(0.05f, 0.44f) * worldRadius;
	unsigned int resolutions[3];
	resolutions[0] = Random::NextInt(1, 20);
	resolutions[1] = Random::NextInt(1, 20);
	resolutions[2] = Random::NextInt(1, 20);
	m_eyes[0].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
	m_eyes[1].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
	
	// Randomize color.
	m_color.x = Random::NextFloat();
	m_color.y = Random::NextFloat();
	m_color.z = Random::NextFloat();
	m_radius = 0.016f; // TODO: magic number agent radius.

	m_wander = true;
	m_moveSpeed = 0.1f; // TEMP: start with some random motion.
	m_turnSpeed = GetSimulation()->GetRandom().NextFloatClamped();
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
	// Update eye matrices.
	float zNear = 0.01f;
	float zFar = m_maxViewDistance;
	float leftEyeCenterAngle = -(m_angleBetweenEyes + m_fieldOfView) * 0.5f;
	Matrix4f eyePerspective = Matrix4f::CreatePerspective(m_eyes[0].GetFieldOfView(), 1.0f, 0.01f, m_maxViewDistance);
	Matrix4f leftEyeRotation = Matrix4f::CreateRotation(Vector3f::UNITY, leftEyeCenterAngle);
	Matrix4f rightEyeRotation = Matrix4f::CreateRotation(Vector3f::UNITY, -leftEyeCenterAngle);
	m_eyes[0].SetEyeToProjection(eyePerspective);
	m_eyes[1].SetEyeToProjection(eyePerspective);
	m_eyes[0].SetWorldToEye(leftEyeRotation * m_worldToObject);
	m_eyes[1].SetWorldToEye(rightEyeRotation * m_worldToObject);

	// Clear all sight values.
	m_eyes[0].ClearSightValues();
	m_eyes[1].ClearSightValues();

	// Query the octtree for objects within vision range.
	Sphere visionSphere(m_position, m_maxViewDistance);
	m_objectManager->GetOctTree()->Query(visionSphere,
		[=](SimulationObject* object)
	{
		if (object != this)
		{
			// Check for collisions with food.
			if (object->GetBoundingSphere().Intersects(GetBoundingSphere()))
			{
				if (object->GetObjectType() == SimulationObjectType::OFFSHOOT)
					EatPlant((Offshoot*) object);
			}

			// Attempt to see the object.
			if (object->IsVisible())
			{
				SeeObject(object);
			}
		}
	});
}

void Agent::EatPlant(Offshoot* plant)
{
	m_energy += plant->Eat();
}

void Agent::SeeObject(SimulationObject* object)
{
	// Discard objects that are too far away.
	if (object->GetPosition().DistToSqr(m_position) >
		m_maxViewDistance * m_maxViewDistance)
		return;

	// Update vision for each eye.
	for (unsigned int eyeIndex = 0; eyeIndex < m_numEyes; ++eyeIndex)
	{
		Retina& eye = m_eyes[eyeIndex];

		// Get the position of the object in the eye's perspective projection.
		Matrix4f worldToEye = eye.GetWorldToEye();
		Vector3f posInEye = worldToEye.ApplyTransform(object->GetPosition());
		Matrix4f projection = eye.GetEyeToProjection();
		Vector3f posInProj1 = projection.ApplyTransform(posInEye - Vector3f(object->GetRadius(), 0, 0));
		Vector3f posInProj2 = projection.ApplyTransform(posInEye + Vector3f(object->GetRadius(), 0, 0));

		// Clip it if is outside the projection.
		if (posInProj1.z <= -1.0f || posInProj1.z >= 1.0f ||
			(posInProj1.x <= -1.0f && posInProj2.x < -1.0f) ||
			(posInProj1.x >= 1.0f && posInProj2.x >= 1.0f))
			continue;

		float t1 = (posInProj1.x + 1.0f) * 0.5f;
		float t2 = (posInProj2.x + 1.0f) * 0.5f;

		// Update the individual channels based on the object's position and color.
		for (unsigned int channel = 0; channel < eye.GetNumChannels(); channel++)
		{
			int index1 = (int) (t1 * eye.GetResolution(channel));
			int index2 = (int) (t2 * eye.GetResolution(channel));
			index1 = Math::Clamp(index1, 0, (int) eye.GetResolution(channel) - 1);
			index2 = Math::Clamp(index2, 0, (int) eye.GetResolution(channel) - 1);

			float depth = posInProj1.z;
			
			for (int index = index1; index <= index2; index++)
			{
				eye.SetSightValue(channel, (unsigned int) index,
					object->GetColor()[channel], depth);
				//eye.SetSightValue(channel, (unsigned int) index,
					//depth, depth);
			}
		}
	}
}

void Agent::UpdateBrain()
{
	// TEMP: random wandering turning.
	if (m_wander)
	{
		//float timeDelta = 0.01666667f;
		float maxTurnSpeed = 6.0f;
	
		float chance = ((Random::NextFloat() * 2) - 1) * maxTurnSpeed;
		float acc = 1;
		if (chance <= m_turnSpeed)
			acc = -1;
		m_turnSpeed += Random::NextFloat() * acc * maxTurnSpeed * 0.1f;
	}
}
