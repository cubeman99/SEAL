#include "Plant.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>

Plant::Plant()
{
	m_isVisible = true;
	m_radius = 0.02f; // TODO: magic number plant radius.
	m_color.Set(1.0f, 1.0f, 0.0f);
}


//Plant::Plant(Simulation* sim, float maxEnergy) :
//	m_simulation(sim),
//	m_maxEnergy(maxEnergy)
//{
//	m_isVisible = false;
//}

Plant::~Plant()
{
}

void Plant::OnSpawn()
{
	// TODO: randomize a bit
	for (int i = 0; i < 6; ++i)
	{
		SpawnOffshoot();
	}
}

void Plant::Update(float timeDelta)
{
	// TODO: spawn offshoots if below capacity with random chance

	if (GetNumOffshoots() > 0)
	{
		for (auto it = m_offshoots.begin(); it != m_offshoots.end(); ++it)
		{
			Offshoot* offshoot = *it;

			offshoot->Update(timeDelta);
		}
	}
	else
	{
		// Respawn with new offshoots
		m_objectManager->CreateRandomPositionAndOrientation(
			m_position, m_orientation);
		OnSpawn();
	}
}

Offshoot* Plant::SpawnOffshoot()
{
	Offshoot* offshoot = new Offshoot(this);
	
	m_offshoots.push_back(offshoot);

	// Spawn the offshoot.
	m_objectManager->SpawnObject(offshoot);
	m_objectManager->CreateRelativeRandomPositionAndOrientation(
		m_position, offshoot->m_position, 0.28f, offshoot->m_orientation);

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