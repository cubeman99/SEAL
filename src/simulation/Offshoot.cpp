#include "Offshoot.h"
#include <simulation/Plant.h>
#include <math/MathLib.h>
#include <simulation/Simulation.h>

Offshoot::Offshoot()
{
}

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

	if (!m_isSerialized)
	{
		m_energy = m_maxEnergy * 0.5f;
	}

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
	m_isSerialized = true;

	fileIn.read((char*)&m_objectId, sizeof(int));
	fileIn.read((char*)&sourceID, sizeof(int));
	fileIn.read((char*)&m_energy, sizeof(float));
	fileIn.read((char*)&m_position, sizeof(Vector3f));
	fileIn.read((char*)&m_orientation, sizeof(Quaternion));

	// Find my Plant source and let it know it's a daddy
	m_source = (Plant*)GetSimulation()->GetObjectManager()->GetObj(sourceID);
	m_source->NotifyOffshootChild(this);
}

void Offshoot::Write(std::ofstream& fileOut)
{
	if (!m_isDestroyed)
	{
		int objType = GetObjectType();
		int sourceID = m_source->GetId();

		fileOut.write((char*)&objType, sizeof(int));
		fileOut.write((char*)&m_objectId, sizeof(int));
		fileOut.write((char*)&sourceID, sizeof(int));
		fileOut.write((char*)&m_energy, sizeof(float));
		fileOut.write((char*)&m_position, sizeof(Vector3f));
		fileOut.write((char*)&m_orientation, sizeof(Quaternion));
	}
}

float Offshoot::Eat()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();
	float amount = config.plant.eatEnergyDepletionRate;
	float energyEaten = Math::Min(m_energy, amount);

	// Deplete the energy.
	m_energy -= amount;
	if (m_energy <= 0.0f)
		Destroy();

	return energyEaten;
}
