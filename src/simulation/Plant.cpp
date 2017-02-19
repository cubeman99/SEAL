#include "Plant.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>


Plant::Plant()
{
}

Plant::~Plant()
{
}

void Plant::OnSpawn()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	m_isVisible = false;
	m_radius = 4;
	m_color.Set(1.0f, 1.0f, 0.0f);

	if (!m_isSerialized)
	{
		for (int i = 0; i < config.plant.numOffshootsPerPlant; ++i)
		{
			SpawnOffshoot();
		}
	}
}

void Plant::Update()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();
	RNG& random = GetSimulation()->GetRandom();

	if (GetNumOffshoots() == 0)
	{
		// Respawn with new offshoots
		m_objectManager->CreateRandomPositionAndOrientation(
			m_position, m_orientation);
		for (int i = 0; i < config.plant.numOffshootsPerPlant; ++i)
			SpawnOffshoot();
	}
	else if (GetNumOffshoots() < config.plant.numOffshootsPerPlant)
	{
		// Spawn offshoot if below capacity with random chance
		if (random.NextInt() % 300 == 0)
		{
			SpawnOffshoot();
		}
	}
}

void Plant::Read(std::ifstream& fileIn)
{
	m_isSerialized = true;

	fileIn.read((char*)&m_objectId, sizeof(int));
	fileIn.read((char*)&m_position, sizeof(Vector3f));
	fileIn.read((char*)&m_orientation, sizeof(Quaternion));
}

void Plant::Write(std::ofstream& fileOut)
{
	if (!m_isDestroyed)
	{
		int objType = GetObjectType();

		fileOut.write((char*)&objType, sizeof(int));
		fileOut.write((char*)&m_objectId, sizeof(int));
		fileOut.write((char*)&m_position, sizeof(Vector3f));
		fileOut.write((char*)&m_orientation, sizeof(Quaternion));
	}
}

Offshoot* Plant::SpawnOffshoot()
{
	const SimulationConfig& config = GetSimulation()->GetConfig();

	Offshoot* offshoot = new Offshoot(this);
	m_offshoots.push_back(offshoot);

	// Spawn the offshoot within a radius around the plant.
	m_objectManager->SpawnObject(offshoot);
	m_objectManager->CreateRelativeRandomPositionAndOrientation(
		m_position, offshoot->m_position,
		config.plant.offshootSpawnRadius,
		offshoot->m_orientation);

	return offshoot;
}

void Plant::NotifyOffshootDeath(Offshoot* toRemove)
{
	// Remove offshoot from my list, but allow ObjectManager to destroy it
	bool foundIt = false;
	for (unsigned int i = 0; i < m_offshoots.size() && !foundIt; ++i)
	{
		if (m_offshoots[i] == toRemove)
		{
			foundIt = true;
			m_offshoots.erase(offshoots_begin() + i);
		}
	}
}

void Plant::NotifyOffshootChild(Offshoot* toAdd)
{
	// For serialization purposes, linking them back together
	m_offshoots.push_back(toAdd);
}