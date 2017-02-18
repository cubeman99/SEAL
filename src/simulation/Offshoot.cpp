#include "Offshoot.h"
#include <simulation/Plant.h>
#include <math/MathLib.h>
#include <simulation/Simulation.h>


Offshoot::Offshoot(Plant* plant) :
	m_source(plant)
{
}

Offshoot::~Offshoot()
{
}

void Offshoot::OnSpawn()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	m_maxEnergy = config.plant.maxEnergy;
	m_growthRate = config.plant.growthRate; // energy per tick
	m_energy = m_maxEnergy * 0.5f;

	m_isVisible = true;
	m_radius = config.plant.radius;
	m_color = Vector3f(
		config.plant.color[0],
		config.plant.color[1],
		config.plant.color[2]);
}

void Offshoot::OnDestroy()
{
	m_source->NotifyOffshootDeath(this);
}

void Offshoot::Update()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	if (m_energy > 0.0f)
	{
		m_energy += m_growthRate;// * timeDelta;

		if (m_energy > m_maxEnergy)
		{
			m_energy = m_maxEnergy;
		}
	}
	else
	{
		Destroy();
	}
	
	// Scale radius based on energy percent.
	float scale = m_energy / m_maxEnergy;
	scale = (0.2f + 0.8f * scale);
	m_radius = config.plant.radius * scale;
}

void Offshoot::Read(std::ifstream& fileIn)
{
	int sourceID;

	fileIn >> m_objectId
		>> sourceID
		>> m_energy
		>> m_position.x
		>> m_position.y
		>> m_position.z
		>> m_orientation.x
		>> m_orientation.y
		>> m_orientation.z
		>> m_orientation.w;

	// Find my Plant source and let it know it's a daddy
	m_source = (Plant*)GetSimulation()->GetObjectManager()->GetObj(sourceID);
	m_source->NotifyOffshootChild(this);
}

void Offshoot::Write(std::ofstream& fileOut)
{
	if (!m_isDestroyed)
	{
		fileOut << GetObjectType()
			<< m_objectId
			<< m_source->GetId()
			<< m_energy
			<< m_position.x
			<< m_position.y
			<< m_position.z
			<< m_orientation.x
			<< m_orientation.y
			<< m_orientation.z
			<< m_orientation.w;
	}
}

float Offshoot::Eat()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();
	float amount = config.plant.eatEnergyDepletionRate;
	float energyEaten = Math::Max(m_energy, amount);

	// Deplete the energy.
	m_energy -= amount;
	if (m_energy <= 0.0f)
		Destroy();

	return energyEaten;
}
