#include "Plant.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>

Plant::Plant(Simulation* sim, float maxEnergy) :
	m_simulation(sim),
	m_maxEnergy(maxEnergy)
{
	m_isVisible = false;
}

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

bool Plant::Update(float growth)
{
	// TODO: spawn offshoots if below capacity with random chance

	for (auto it = m_offshoots.begin(); it != m_offshoots.end();)
	{
		Offshoot* offshoot = *it;

		bool isDead = offshoot->Update(growth);

		if (isDead)
		{
			it = m_offshoots.erase(it);

			// TODO: remove from octree?
		}
		else
		{
			++it;
		}

		m_simulation->GetOctTree()->DynamicUpdate(offshoot);
	}

	return (GetNumOffshoots() == 0); // If all used up
}

Offshoot* Plant::SpawnOffshoot()
{
	Offshoot* offshoot = new Offshoot(m_maxEnergy);

	float worldRadius = m_simulation->GetWorld()->GetRadius();

	// Fudge position away from plant source
	float factor = 0.25f;
	offshoot->m_position = m_position;
	offshoot->m_position.x += Random::NextFloat(-1, 1) * factor;
	offshoot->m_position.y += Random::NextFloat(-1, 1) * factor;
	offshoot->m_position.z += Random::NextFloat(-1, 1) * factor;
	offshoot->m_position.Normalize();
	offshoot->m_position *= worldRadius;

	// Randomize orientation (tangent to world surface).
	Vector3f axis = offshoot->m_position.Cross(Vector3f::UP).Normalize();
	float angle = Math::ACos(Vector3f::Normalize(offshoot->m_position).Dot(Vector3f::UP));
	offshoot->m_orientation = Quaternion(axis, angle);
	offshoot->m_orientation.Rotate(offshoot->m_orientation.GetUp(),
		Random::NextFloat() * Math::TWO_PI);

	m_offshoots.push_back(offshoot);

	// Insert the agent into the oct-tree.
	m_simulation->GetOctTree()->InsertObject(offshoot);

	return offshoot;
}