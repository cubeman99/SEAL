#include "Agent.h"
#include "graphics/ParticleSystem.h"
#include <utilities/Random.h>
#include <simulation/ObjectManager.h>
#include <simulation/Simulation.h>
#include <math/MathLib.h>
#include <math/Vector2f.h>


//-----------------------------------------------------------------------------
// Constructors & destructor
//-----------------------------------------------------------------------------

// Adam and Eve constructor
Agent::Agent(Species species) :
	m_moveSpeed(0.0f),
	m_turnSpeed(0.0f),
	m_numEyes(2),
	m_genome(nullptr),
	m_brain(nullptr),
	m_energyUsage(0.0f),
	m_species(species)
{
	m_inOrbit = 0.0f;
}

// Natural born constructor
Agent::Agent(Genome* genome, float energy, Species species) :
	m_moveSpeed(0.0f),
	m_turnSpeed(0.0f),
	m_numEyes(2),
	m_genome(genome),
	m_energy(energy),
	m_healthEnergy(energy),
	m_brain(nullptr),
	m_species(species)
{
	m_inOrbit = 0.0f;
}

Agent::~Agent()
{
	delete m_brain;
	m_brain = nullptr;
	delete m_genome;
	m_genome = nullptr;
}


//-----------------------------------------------------------------------------
// Simulation object methods
//-----------------------------------------------------------------------------

void Agent::OnSpawn()
{
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);

	m_manualOverride = false;

	bool adamAndEve = false;

	// Don't ovverwrite these values if they've already been read in
	if (!m_isSerialized)
	{
		m_moveSpeed		= 0.0f;
		m_turnSpeed		= 0.0f;
		m_age			= 0;
		m_fitness		= 0.0f;
		m_mateWaitTime = config.agent.matingDelay; // Don't allow mating initially.
	}

	// If the genome is null, then create a randomized one.
	// This is a sign that this agent has no parents.
	if (m_genome == nullptr)
	{
		m_genome = new Genome(config);
		m_genome->Randomize(GetSimulation()->GetRandom());
		adamAndEve = true;
	}

	// May already be read() in
	if (!m_isSerialized)
	{
		// Grow the brain from the genome.
		m_brain = new Brain();
		m_genome->GrowBrain(m_brain, config, GetSimulation()->GetRandom());
		m_brain->PreBirth(config.brain.numPrebirthCycles,
			GetSimulation()->GetRandom());
	}

	// Determine agent properties based on gene values.
	m_strength = m_genome->GetGeneAsFloat(GenePosition::STRENGTH);
	m_lifeSpan = m_genome->GetGeneAsInt(
		GenePosition::LIFE_SPAN,
		config.genes.minLifeSpan,
		config.genes.maxLifeSpan);
	m_desiredNumChildren = m_genome->GetGeneAsInt(
		GenePosition::CHILD_COUNT,
		config.genes.minChildren,
		config.genes.maxChildren);
	m_mutationRate = m_genome->GetGeneAsFloat(
		GenePosition::MUTATION_RATE,
		config.genes.minMutationRate,
		config.genes.maxMutationRate);
	m_numCrossoverPoints = m_genome->GetGeneAsInt(
		GenePosition::CROSSOVER_POINTS,
		config.genes.minCrossoverPoints,
		config.genes.maxCrossoverPoints);
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

	// Configure eyes.
	m_eyes[0].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);
	m_eyes[1].Configure(m_fieldOfView, m_maxViewDistance, 3, resolutions);

	if (adamAndEve)
	{
		m_energy = m_maxEnergy * 0.70f;
		m_healthEnergy = m_energy;
	}
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
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);

	if (GetInOrbit())
	{
		// Fall
		m_inOrbit -= 0.005f;
		m_orientation.Rotate(m_orientation.GetUp(), 0.25f);
		m_position.Normalize();
		m_position *= GetSimulation()->GetWorld()->GetRadius() * m_inOrbit;
		return;
	}

	UpdateVision();
	UpdateBrain();

	// Turn and move.
	m_orientation.Rotate(m_orientation.GetUp(), m_turnSpeed);
	m_objectManager->MoveObjectForward(this, m_moveSpeed);

	// Update energy usage.
	m_energyUsage = config.energy.energyCostExist +
		(config.energy.energyCostMove * m_moveSpeed) +
		(config.energy.energyCostTurn * m_turnSpeed) +
		(config.energy.energyCostNeuron * m_brain->GetNumNeurons()) +
		(config.energy.energyCostSynapse * m_brain->GetNumSynapses());
	m_energy -= m_energyUsage;

	m_age++;

	if (m_mateWaitTime > 0)
		m_mateWaitTime--;

	// Kill agent after lifetime expiration.
	if (m_age > m_lifeSpan)
	{
		Die();
	}

	// Kill agent if its energy is depleted.
	if (m_energy <= 0.0f)
	{
		m_energy = 0.0f;
		Die();
	}
}

void Agent::Read(std::ifstream& fileIn)
{
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);

	m_isSerialized = true;
	m_genome = new Genome(config);
	m_brain = new Brain();

	// Read basic info
	int speciesIndex;
	fileIn.read((char*)&m_objectId, sizeof(int));
	fileIn.read((char*)&m_position, sizeof(Vector3f));
	fileIn.read((char*)&m_orientation, sizeof(Quaternion));
	fileIn.read((char*)&speciesIndex, sizeof(int));
	fileIn.read((char*)&m_moveSpeed, sizeof(float));
	fileIn.read((char*)&m_turnSpeed, sizeof(float));
	fileIn.read((char*)&m_energy, sizeof(float));
	fileIn.read((char*)&m_healthEnergy, sizeof(float));
	fileIn.read((char*)&m_age, sizeof(int));
	fileIn.read((char*)&m_fitness, sizeof(float));
	fileIn.read((char*)&m_inOrbit, sizeof(float));
	fileIn.read((char*)&m_mateWaitTime, sizeof(int));
	m_species = (Species) speciesIndex;

	// Read genome
	fileIn.read((char*)m_genome->GetData(),
		m_genome->GetSize() * sizeof(unsigned char));

	// Read brain
	fileIn.read((char*)&m_brain->m_numNeurons, sizeof(unsigned int));
	fileIn.read((char*)&m_brain->m_numInputNeurons, sizeof(unsigned int));
	fileIn.read((char*)&m_brain->m_numOutputNeurons, sizeof(unsigned int));
	fileIn.read((char*)&m_brain->m_numSynapses, sizeof(unsigned int));
	fileIn.read((char*)&m_brain->m_decayRate, sizeof(float));
	fileIn.read((char*)&m_brain->m_maxWeight, sizeof(float));

	// Allocate neuron and synapse arrays
	m_brain->m_neurons = new Neuron[m_brain->m_numNeurons];
	m_brain->m_currNeuronActivations = new float[m_brain->m_numNeurons];
	m_brain->m_prevNeuronActivations = new float[m_brain->m_numNeurons];
	m_brain->m_synapses = new Synapse[m_brain->m_numSynapses];

	// Read neuron and synapse data
	fileIn.read((char*)m_brain->m_neurons, m_brain->m_numNeurons * sizeof(Neuron));
	fileIn.read((char*)m_brain->m_synapses, m_brain->m_numSynapses * sizeof(Synapse));
	fileIn.read((char*)m_brain->m_currNeuronActivations, m_brain->m_numNeurons * sizeof(float));
}

void Agent::Write(std::ofstream& fileOut)
{
	if (!m_isDestroyed)
	{
		int objType = GetObjectType();

		int speciesIndex = (int) m_species;

		// Write basic info
		fileOut.write((char*)&objType, sizeof(int));
		fileOut.write((char*)&m_objectId, sizeof(int));
		fileOut.write((char*)&m_position, sizeof(Vector3f));
		fileOut.write((char*)&m_orientation, sizeof(Quaternion));
		fileOut.write((char*)&speciesIndex, sizeof(int));
		fileOut.write((char*)&m_moveSpeed, sizeof(float));
		fileOut.write((char*)&m_turnSpeed, sizeof(float));
		fileOut.write((char*)&m_energy, sizeof(float));
		fileOut.write((char*)&m_healthEnergy, sizeof(float));
		fileOut.write((char*)&m_age, sizeof(int));
		fileOut.write((char*)&m_fitness, sizeof(float));
		fileOut.write((char*)&m_inOrbit, sizeof(float));
		fileOut.write((char*)&m_mateWaitTime, sizeof(int));

		// Write genome
		fileOut.write((char*)m_genome->GetData(), m_genome->GetSize() * sizeof(unsigned char));

		// Write brain
		fileOut.write((char*)&m_brain->m_numNeurons, sizeof(unsigned int));
		fileOut.write((char*)&m_brain->m_numInputNeurons, sizeof(unsigned int));
		fileOut.write((char*)&m_brain->m_numOutputNeurons, sizeof(unsigned int));
		fileOut.write((char*)&m_brain->m_numSynapses, sizeof(unsigned int));
		fileOut.write((char*)&m_brain->m_decayRate, sizeof(float));
		fileOut.write((char*)&m_brain->m_maxWeight, sizeof(float));
		fileOut.write((char*)m_brain->m_neurons,
			m_brain->m_numNeurons * sizeof(Neuron));
		fileOut.write((char*)m_brain->m_synapses,
			m_brain->m_numSynapses * sizeof(Synapse));
		fileOut.write((char*)m_brain->m_currNeuronActivations,
			m_brain->m_numNeurons * sizeof(float));
	}
}


//-----------------------------------------------------------------------------
// Agent methods
//-----------------------------------------------------------------------------

void Agent::UpdateVision()
{
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);

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
	
	std::vector<Agent*> agentMateCollisions;
	std::vector<Agent*> agentCollisions;

	//bool canEatPlants = (m_species == SPECIES_HERBIVORE);
	bool canEatPlants = true;
	bool canMate = (m_mateWaitTime == 0 && GetSimulation()->IsMatingSeason());

	// Query the octtree for objects within vision range.
	Sphere visionSphere(m_position, m_maxViewDistance);
	m_objectManager->GetOctTree()->Query(visionSphere,
		[&](SimulationObject* object)
	{
		if (object != this && !object->GetInOrbit())
		{
			float contactDist = m_radius + object->GetRadius();
			float matingDist = config.agent.minMatingDistance;
			float distSqr = m_position.DistToSqr(object->GetPosition());

			// TODO: Move collisions outside of UpdateVision()

			// Check for collisions with food.
			if (object->GetObjectType() == SimulationObjectType::OFFSHOOT &&
				distSqr < contactDist * contactDist && canEatPlants)
			{
				EatPlant((Offshoot*) object);
			}

			// Check for mating interations with other agents.
			if (object->GetObjectType() == SimulationObjectType::AGENT &&
				distSqr < matingDist * matingDist && canMate)
			{
				// put into an array to avoid concurrent modification of octtree
				agentMateCollisions.push_back((Agent*) object);
			}

			// Check for collision with another agent.
			if (object->GetObjectType() == SimulationObjectType::AGENT &&
				distSqr < contactDist * contactDist)
			{
				agentCollisions.push_back((Agent*) object);
			}

			// Attempt to see the object.
			if (object->IsVisible())
			{
				SeeObject(object);
			}
		}
	});

	for (unsigned int i = 0; i < agentCollisions.size(); ++i)
	{
		OnTouchAgent(agentCollisions[i]);
	}

	// Attempt to mate with detected nearby agents.
	for (unsigned int i = 0; i < agentMateCollisions.size(); ++i)
	{
		Mate(agentMateCollisions[i]);
	}
}

void Agent::EatPlant(Offshoot* plant)
{
	if (m_energy < m_maxEnergy)
	{
		const SimulationConfig& config = GetSimulation()->GetConfig();

		if (m_species == SPECIES_HERBIVORE)
		{
			float eatAmount = Math::Min(config.plant.eatEnergyDepletionRate, m_maxEnergy - m_energy);
			float energyAmount = plant->Eat(eatAmount);
			m_energy = Math::Min(m_energy + energyAmount, m_maxEnergy);
			m_healthEnergy = Math::Min(m_healthEnergy + energyAmount, m_maxEnergy);
			m_fitness += energyAmount;
		}
		else
		{
			plant->Eat(config.plant.eatEnergyDepletionRate);
		}
	}
}

void Agent::OnTouchAgent(Agent* other)
{
	if (m_species == SPECIES_CARNIVORE &&
		other->m_species == SPECIES_HERBIVORE)
	{
		Attack(other);
	}
}

void Agent::Attack(Agent* other)
{
	// Calculate attack damage, lowered by herbivore strength
	float attackAmount = m_strength * 2.0f * ((1.0f - other->m_strength) + 0.05f);

	other->m_healthEnergy -= attackAmount;

	if (other->m_healthEnergy <= 0.0f)
	{
		// Death particles
		GetSimulation()->AddParticles(12, ParticleType::AGENT_KILLED, other->m_position);

		float toGain = other->m_maxEnergy / 2.0f;
		m_fitness += toGain;
		m_energy = Math::Min(m_energy + toGain, m_maxEnergy);
		other->m_healthEnergy = 0.0f;
		other->Die();
	}
}

void Agent::Die()
{
	float ageFitnessParam = 1.0f / 60.0f; // 1 fitness per second
	float energyFitnessParam = 2.0f;

	m_fitness += m_age * ageFitnessParam;
	m_fitness += m_energy * energyFitnessParam;
	
	GetSimulation()->OnAgentDie(this);

	Destroy();
}

void Agent::Mate(Agent* mate)
{
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);

	// Make sure only one agent of the pair will call this method.
	if (GetId() < mate->GetId() || m_species != mate->m_species)
		return;

	float energyPercentAtMinChildren = 0.2f;
	float energyPercentAtMaxChildren = 0.6f;

	Agent* parents[2] = { this, mate };

	// Calculate the required energy per child.
	float avgNumChildrenGene =
		(parents[0]->GetGenome()->GetGeneAsFloat(GenePosition::CHILD_COUNT) +
		parents[1]->GetGenome()->GetGeneAsFloat(GenePosition::CHILD_COUNT)) * 0.5f;

	int maxNumChildren = (int) (Math::Lerp((float) config.genes.minChildren,
		(float) config.genes.maxChildren, avgNumChildrenGene) + 0.5f);
	if (maxNumChildren < 1)
		maxNumChildren = 1;
	float maxEnergyPercent = Math::Lerp(energyPercentAtMinChildren,
		energyPercentAtMaxChildren, avgNumChildrenGene);
	float maxTransferrableEnergy = (parents[0]->GetMaxEnergy() +
		parents[1]->GetMaxEnergy()) * maxEnergyPercent;
	float energyPerChild = maxTransferrableEnergy / (float) maxNumChildren;

	// Calculate the amount of energy each parent has availableo to
	// transfer to their children.
	float transferrableEnergy[2];
	transferrableEnergy[0] = parents[0]->GetEnergy() * maxEnergyPercent;
	transferrableEnergy[1] = parents[1]->GetEnergy() * maxEnergyPercent;
	float actualTransferrableEnergy = transferrableEnergy[0] + transferrableEnergy[1];
	if (actualTransferrableEnergy < energyPerChild)
		return;

	// Determine the actual number of children that can be born with
	// the amount of available transferrable energy.
	int actualNumChildren = (int) (actualTransferrableEnergy / energyPerChild);
	actualNumChildren = Math::Clamp(actualNumChildren, 0, maxNumChildren);
	if (actualNumChildren == 0)
	{
		// Small delay to decrease the unnecessary use of this function
		// TODO: magic numbers
		parents[0]->m_mateWaitTime = 10;
		parents[1]->m_mateWaitTime = 10;
		return;
	}


	// Subtract the transferred energy for each parent.
	parents[0]->m_energy -= (transferrableEnergy[0] / actualTransferrableEnergy) * energyPerChild * actualNumChildren;
	parents[1]->m_energy -= (transferrableEnergy[1] / actualTransferrableEnergy) * energyPerChild * actualNumChildren;
	parents[0]->m_mateWaitTime = config.agent.matingDelay;
	parents[1]->m_mateWaitTime = config.agent.matingDelay;

	// Mated particles
	GetSimulation()->AddParticles(12, ParticleType::AGENT_MATED, m_position);

	// Spawn the children.
	for (int i = 0; i < actualNumChildren; ++i)
	{
		// Crossover and mutate the genome.
		Genome* childGenome = Genome::SpawnChild(
			parents[0]->GetGenome(), parents[1]->GetGenome(),
			config, GetSimulation()->GetRandom());
		Agent* child = new Agent(childGenome, energyPerChild, m_species);

		// Set the child's position and orientation.
		Vector3f childPos = (parents[0]->GetPosition() +
			parents[1]->GetPosition()) * 0.5f;
		childPos.Normalize();
		childPos *= GetSimulation()->GetWorld()->GetRadius();
		child->SetPosition(childPos);
		Quaternion childOrientation;
		m_objectManager->CreateRandomOrientation(childPos, childOrientation);
		child->SetOrientation(childOrientation);
		
		child->SetPosition(m_position);
		child->SetOrientation(m_orientation);

		// Spawn the child into the world.
		m_objectManager->SpawnObject(child);
	}
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
	const SpeciesConfig& config = GetSimulation()->GetAgentConfig(m_species);
	RNG& random = GetSimulation()->GetRandom();

	//-------------------------------------------------------------------------
	// Set the input nerve activations.

	for (unsigned int i = 0; i < m_brain->GetNumInputNeurons(); ++i)
		m_brain->SetNeuronActivation(i, 0.0f);

	m_brain->SetNeuronActivation((unsigned int)CURRENT_ENERGY, m_energy / m_maxEnergy);
	m_brain->SetNeuronActivation((unsigned int)RANDOM_ACTIVATION, random.NextFloat());
	m_brain->SetNeuronActivation((unsigned int)IS_MATING_SEASON, (float)GetSimulation()->IsMatingSeason());
	
	for (unsigned int eyeIndex = 0; eyeIndex < m_numEyes; ++eyeIndex)
	{
		Retina& eye = m_eyes[eyeIndex];
		for (unsigned int channel = 0; channel < eye.GetNumChannels(); ++channel)
		{
			for (unsigned int i = 0; i < eye.GetResolution(channel); ++i)
			{
				unsigned int inputIndex = NUM_NON_SIGHT_INPUTS + (((channel * 2) + eyeIndex) *
					config.genes.maxSightResolution) + i;
				m_brain->SetNeuronActivation(inputIndex,
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
			m_brain->GetNumInputNeurons() + MOVE_SPEED);
		float turnAmount = m_brain->GetNeuronActivation(
			m_brain->GetNumInputNeurons() + TURN_SPEED);

		m_moveAmount = moveAmount;
		m_turnAmount = (turnAmount * 2.0f) - 1.0f;
		m_moveSpeed = m_moveAmount * m_maxMoveSpeed;
		m_turnSpeed = m_turnAmount * m_maxTurnSpeed;
	}
}
