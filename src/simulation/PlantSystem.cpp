#include "PlantSystem.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>


PlantSystem::PlantSystem(Simulation* simulation, float growthRate, float maxEnergy) :
	m_simulation(simulation),
	m_growthRate(growthRate),
	m_maxEnergy(maxEnergy)
{
	for (int i = 0; i < 5; ++i)
	{
		SpawnPlant();
	}
}

PlantSystem::~PlantSystem()
{
	RemoveAllPlants();
}

void PlantSystem::RemoveAllPlants()
{
	for (auto it = m_plants.begin(); it != m_plants.end(); ++it)
		delete *it;
	m_plants.clear();
}

Plant* PlantSystem::SpawnPlant()
{
	Plant* plant = new Plant(m_simulation, m_maxEnergy);

	float worldRadius = m_simulation->GetWorld()->GetRadius();
	
	// Randomize position (on world surface).
	plant->m_position.x = Random::NextFloat(-1, 1);
	plant->m_position.y = Random::NextFloat(-1, 1);
	plant->m_position.z = Random::NextFloat(-1, 1);
	plant->m_position.Normalize();
	plant->m_position *= worldRadius;

	m_plants.push_back(plant);

	// Insert the agent into the oct-tree.		// TODO: make this not crash
	//m_simulation->GetOctTree()->InsertObject(plant);

	return plant;
}

void PlantSystem::UpdatePlants(float timeDelta)
{
	for (auto it = m_plants.begin(); it != m_plants.end(); ++it)
	{
		Plant* plant = *it;

		bool usedUp = plant->Update(timeDelta * m_growthRate);

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
		}
	}
}