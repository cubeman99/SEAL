#include "Agent.h"
#include <utilities/Random.h>
#include <simulation/ObjectManager.h>
#include <simulation/Simulation.h>
#include <math/MathLib.h>
#include <math/Vector2f.h>

// Adam and Eve constructor
Agent::Agent() :
	m_moveSpeed(0.0f),
	m_turnSpeed(0.0f),
	m_numEyes(2),
	m_genome(nullptr),
	m_brain(nullptr)
{
}

// Natural born constructor
Agent::Agent(Genome* genome, float energy) :
	m_moveSpeed(0.0f),
	m_turnSpeed(0.0f),
	m_numEyes(2),
	m_genome(genome),
	m_energy(energy)
{
}

Agent::~Agent()
{
}

void Agent::OnSpawn()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	m_radius = config.agent.radius;
	m_manualOverride = false;
	m_moveSpeed = 0.0f;
	m_turnSpeed = 0.0f;
	m_age = 0;
	m_fitness = 0.0f;

	// If the genome is null, then create a randomized one.
	// This is a sign that this agent is a first gen with no parents.
	if (m_genome == nullptr)
	{
		m_genome = new Genome(GetSimulation(), true);
		m_energy = 100.0f;
	}

	// Grow the brain from the genome.
	m_brain = new Brain();
	m_genome->GrowBrain(m_brain);
	m_brain->PreBirth(config.brain.numPrebirthCycles,
		GetSimulation()->GetRandom());

	// Determine agent properties based on gene values.
	m_strength = m_genome->GetGeneAsFloat(GenePosition::STRENGTH);
	m_lifeSpan = m_genome->GetGeneAsInt(
		GenePosition::LIFE_SPAN,
		config.genes.minLifeSpan,
		config.genes.maxLifeSpan);
	m_angleBetweenEyes = m_genome->GetGeneAsFloat(
		GenePosition::ANGLE_BETWEEN_EYES,
		config.genes.minAngleBetweenEyes,
		config.genes.maxAngleBetweenEyes);
	m_maxViewDistance = m_genome->GetGeneAsFloat(
		GenePosition::VIEW_DISTANCE,
		config.genes.minSightDistance,
		config.genes.maxSightDistance);
	m_fieldOfView = m_genome->GetGeneAsFloat(
		GenePosition::FIELD_OF_VIEW,
		config.genes.minFieldOfView,
		config.genes.maxFieldOfView);
	unsigned int resolutions[3];
	resolutions[0] = m_genome->GetGeneAsInt(
		GenePosition::RESOLUTION_RED,
		config.genes.minSightResolution,
		config.genes.maxSightResolution);
	resolutions[1] = m_genome->GetGeneAsInt(
		GenePosition::RESOLUTION_GREEN,
		config.genes.minSightResolution,
		config.genes.maxSightResolution);
	resolutions[2] = m_genome->GetGeneAsInt(
		GenePosition::RESOLUTION_BLUE,
		config.genes.minSightResolution,
		config.genes.maxSightResolution);
	m_color[0] = m_genome->GetGeneAsFloat(
		GenePosition::COLOR_RED,
		config.genes.minBodyColor[0],
		config.genes.maxBodyColor[0]);
	m_color[1] = m_genome->GetGeneAsFloat(
		GenePosition::COLOR_GREEN,
		config.genes.minBodyColor[1],
		config.genes.maxBodyColor[1]);
	m_color[2] = m_genome->GetGeneAsFloat(
		GenePosition::COLOR_BLUE,
		config.genes.minBodyColor[2],
		config.genes.maxBodyColor[2]);

	// Determine values derived from strength.
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
	m_radius = Math::Lerp(
		config.agent.radiusAtMinStrength,
		config.agent.radiusAtMaxStrength,
		m_strength);

	m_energy = m_maxEnergy * 0.8f;

	// Configure eyes.
	m_eyes[0].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
	m_eyes[1].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
}

void Agent::OnDestroy()
{
	// Clean up my components.
	delete m_brain;
	m_brain = nullptr;
	delete m_genome;
	m_genome = nullptr;
}

void Agent::Update()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

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

	// Update energy costs.
	m_energy -= config.energy.energyCostExist;
	m_energy -= config.energy.energyCostMove * m_moveSpeed;
	m_energy -= config.energy.energyCostTurn * m_turnSpeed;

	if (m_energy <= 0.0f)
	{
		m_energy = 0.0f;

		// TODO: kill agent after energy depletion.
		//Destroy();
	}
}

void Agent::UpdateVision()
{
	// Update eye matrices.
	float zNear = 0.01f;
	float zFar = m_maxViewDistance;
	float centerAngle = (m_angleBetweenEyes + m_fieldOfView) * 0.5f;
	Matrix4f eyePerspective = Matrix4f::CreatePerspective(
		m_eyes[0].GetFieldOfView(), 1.0f, m_radius * 0.1f, m_maxViewDistance);
	Matrix4f leftEyeRotation = Matrix4f::CreateRotation(
		Vector3f::UNITY, -centerAngle);
	Matrix4f rightEyeRotation = Matrix4f::CreateRotation(
		Vector3f::UNITY, centerAngle);
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
	// TODO: uncomment when agents lose energy
	//if (m_energy < m_maxEnergy)
	//{

	float energyAmount = plant->Eat();
		m_energy += energyAmount;
		m_energy = Math::Clamp(m_energy, 0.0f, m_maxEnergy);
		m_fitness += energyAmount;
	//}

}

void Agent::SeeObject(SimulationObject* object)
{
	float distSqr = object->GetPosition().DistToSqr(m_position);

	// Discard objects that are too far away.
	if (distSqr > m_maxViewDistance * m_maxViewDistance)
		return;

	// Check if we are inside the object. If so, then the object's
	// color should fill the entirety of our vision strip.
	bool isInsideMe = (distSqr <= object->GetRadius() * object->GetRadius());

	// Update vision for each eye.
	for (unsigned int eyeIndex = 0; eyeIndex < m_numEyes; ++eyeIndex)
	{
		Retina& eye = m_eyes[eyeIndex];

		float t1 = 0.0f;
		float t2 = 1.0f;
		float depth = 0.0f;

		if (!isInsideMe)
		{
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
			
			t1 = (posInProj1.x + 1.0f) * 0.5f;
			t2 = (posInProj2.x + 1.0f) * 0.5f;
			depth = posInProj1.z;
		}

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
	const SimulationConfig& config = GetSimulation()->GetConfig();
	RNG& random = GetSimulation()->GetRandom();

	//-------------------------------------------------------------------------
	// Set the input nerve activations.

	for (unsigned int i = 0; i < m_brain->GetNumInputNeurons(); ++i)
		m_brain->SetNeuronActivation(i, 0.0f);

	m_brain->SetNeuronActivation(0, m_energy / m_maxEnergy);
	m_brain->SetNeuronActivation(1, random.NextFloat());
	
	for (unsigned int eyeIndex = 0; eyeIndex < m_numEyes; ++eyeIndex)
	{
		Retina& eye = m_eyes[eyeIndex];
		for (unsigned int channel = 0; channel < eye.GetNumChannels(); ++channel)
		{
			for (unsigned int i = 0; i < eye.GetResolution(channel); ++i)
			{
				unsigned int geneIndex = 2 + (((channel * 2) + eyeIndex) *
					config.genes.maxSightResolution) + i;
				m_brain->SetNeuronActivation(geneIndex,
					eye.GetSightValueAtIndex(channel, i));
			}
		}
	}

	//-------------------------------------------------------------------------
	// Update the brain's neural network.

	m_brain->Update();

	//-------------------------------------------------------------------------
	// Get the output nerve activations.

	if (!m_manualOverride)
	{
		float moveAmount = m_brain->GetNeuronActivation(
			m_brain->GetNumInputNeurons() + 0);
		float turnAmount = m_brain->GetNeuronActivation(
			m_brain->GetNumInputNeurons() + 1);
		m_moveSpeed = moveAmount * m_maxMoveSpeed;
		m_turnSpeed = ((turnAmount * 2) - 1) * m_maxTurnSpeed;
	}
}
