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

static float GeneLerp(float gene, float minValue, float maxValue)
{
	return Math::Lerp(minValue, maxValue, gene);
}

static int GeneLerp(float gene, int minValue, int maxValue)
{
	return (int) (Math::Lerp((float) minValue, (float) maxValue, gene) + 0.5f);
}

void Agent::OnSpawn()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	m_radius = config.agent.radius;
	m_energy = 100.0f;
	m_wander = true;
	m_moveSpeed = 0.0f;
	m_turnSpeed = 0.0f;
	m_age = 0;

	// Use some random gene values for now (all must be between 0 and 1).
	RNG& random = GetSimulation()->GetRandom();
	float geneStrength = random.NextFloat();
	float geneLifeSpan = random.NextFloat();
	float geneAngleBetweenEyes = random.NextFloat();
	float geneFieldOfView = random.NextFloat();
	float geneSightDistance = random.NextFloat();
	float geneResolutionRed = random.NextFloat();
	Vector3f geneResolutions(random.NextFloat(),
		random.NextFloat(), random.NextFloat());
	Vector3f geneColors(random.NextFloat(),
		random.NextFloat(), random.NextFloat());

	// Determine agent properties based on the gene values.
	// The Genome class should have lerp functions for
	// float/int genes just for this purpose.
	m_strength = geneStrength;
	m_lifeSpan = GeneLerp(geneLifeSpan,
		config.genes.minLifeSpan,
		config.genes.maxLifeSpan);
	m_angleBetweenEyes = GeneLerp(geneAngleBetweenEyes,
		config.genes.minAngleBetweenEyes,
		config.genes.maxAngleBetweenEyes);
	m_maxViewDistance = GeneLerp(geneSightDistance,
		config.genes.minSightDistance,
		config.genes.maxSightDistance);
	m_fieldOfView = GeneLerp(geneAngleBetweenEyes, 
		config.genes.minFieldOfView,
		config.genes.maxFieldOfView);
	unsigned int resolutions[3];
	resolutions[0] = GeneLerp(geneResolutions[0],
			config.genes.minSightResolution,
			config.genes.maxSightResolution);
	resolutions[1] = GeneLerp(geneResolutions[1],
			config.genes.minSightResolution,
			config.genes.maxSightResolution);
	resolutions[2] = GeneLerp(geneResolutions[2],
			config.genes.minSightResolution,
			config.genes.maxSightResolution);
	m_color[0] = GeneLerp(geneColors[0],
			config.genes.minBodyColor[0],
			config.genes.maxBodyColor[0]);
	m_color[1] = GeneLerp(geneColors[1],
			config.genes.minBodyColor[1],
			config.genes.maxBodyColor[1]);
	m_color[2] = GeneLerp(geneColors[2],
			config.genes.minBodyColor[2],
			config.genes.maxBodyColor[2]);

	// Derive values based on strength.
	m_maxEnergy = Math::Lerp(
						config.agent.maxEnergyAtMinStrength,
						config.agent.maxEnergyAtMaxStrength,
						m_strength);
	m_maxMoveSpeed = Math::Lerp(
						config.agent.maxMoveSpeedAtMinStrength,
						config.agent.maxMoveSpeedAtMaxStrength,
						m_strength);
	m_maxTurnSpeed = Math::Lerp(
						config.agent.maxTurnSpeedAtMinStrength,
						config.agent.maxTurnSpeedAtMaxStrength,
						m_strength);

	// Configure eyes.
	m_eyes[0].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
	m_eyes[1].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
}

void Agent::Update()
{
	UpdateVision();
	UpdateBrain();

	m_age++;
	if (m_age > m_lifeSpan)
	{
		// TODO: kill agent after lifetime expiration.
		//Destroy();
	}

	// Turn and move.
	m_orientation.Rotate(m_orientation.GetUp(), m_turnSpeed);
	m_objectManager->MoveObjectForward(this, m_moveSpeed);
}

void Agent::UpdateVision()
{
	// Update eye matrices.
	float zNear = 0.01f;
	float zFar = m_maxViewDistance;
	float leftEyeCenterAngle = -(m_angleBetweenEyes + m_fieldOfView) * 0.5f;
	Matrix4f eyePerspective = Matrix4f::CreatePerspective(
		m_eyes[0].GetFieldOfView(), 1.0f, m_radius * 0.1f, m_maxViewDistance);
	Matrix4f leftEyeRotation = Matrix4f::CreateRotation(
		Vector3f::UNITY, leftEyeCenterAngle);
	Matrix4f rightEyeRotation = Matrix4f::CreateRotation(
		Vector3f::UNITY, -leftEyeCenterAngle);
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
			// TODO: Move collisions outside of UpdateVision()
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
		Vector3f posInProj1 = projection.ApplyTransform(posInEye -
			Vector3f(object->GetRadius(), 0, 0));
		Vector3f posInProj2 = projection.ApplyTransform(posInEye +
			Vector3f(object->GetRadius(), 0, 0));

		// Clip it if is outside the projection.
		if (posInProj1.z <= -1.0f || posInProj1.z >= 1.0f ||
			(posInProj1.x <= -1.0f && posInProj2.x < -1.0f) ||
			(posInProj1.x >= 1.0f && posInProj2.x >= 1.0f))
			continue;

		float t1 = (posInProj1.x + 1.0f) * 0.5f;
		float t2 = (posInProj2.x + 1.0f) * 0.5f;
		float depth = posInProj1.z;

		// Update the individual channels based on the object's position and color.
		for (unsigned int channel = 0; channel < eye.GetNumChannels(); channel++)
		{
			int index1 = (int) (t1 * eye.GetResolution(channel));
			int index2 = (int) (t2 * eye.GetResolution(channel));
			index1 = Math::Clamp(index1, 0, (int) eye.GetResolution(channel) - 1);
			index2 = Math::Clamp(index2, 0, (int) eye.GetResolution(channel) - 1);

			for (int index = index1; index <= index2; index++)
			{
				eye.SetSightValue(channel, (unsigned int) index,
					object->GetColor()[channel], depth);
			}
		}
	}
}

void Agent::UpdateBrain()
{
	// TODO: Actually update brain and get its output values.
	//float turnAmount = 0.0f; // output from brain
	float moveAmount = 1.0f; // output from brain
	m_moveSpeed = moveAmount * m_maxMoveSpeed;
	//m_turnSpeed = ((turnAmount * 2) - 1) * m_maxTurnSpeed;

	// TEMP: random wandering turning.
	if (m_wander)
	{
		float maxTurnSpeed = 6.0f / 60.0f;
		float chance = ((Random::NextFloat() * 2) - 1) * maxTurnSpeed;
		float acc = 1;
		if (chance <= m_turnSpeed)
			acc = -1;
		m_turnSpeed += Random::NextFloat() * acc * maxTurnSpeed * 0.1f;
	}
}
