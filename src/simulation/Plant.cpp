#include "Plant.h"
#include "Simulation.h"
#include <math/MathLib.h>
#include <utilities/Random.h>

Plant::Plant(Simulation* sim, float maxEnergy) :
	theSimulation(sim),
	m_maxEnergy(maxEnergy)
{
	for (int i = 0; i < 5; ++i)
	{
		SpawnOffshoot();
	}
}

Plant::~Plant()
{
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

		theSimulation->GetOctTree()->DynamicUpdate(offshoot);
	}

	return (GetNumOffshoots() == 0); // If all used up
}

Offshoot* Plant::SpawnOffshoot()	// TODO: Spawn only within radius of me
{
	Offshoot* offshoot = new Offshoot(m_maxEnergy);

	float worldRadius = theSimulation->GetWorld()->GetRadius();

	// Randomize position (on world surface).
	offshoot->m_position.x = Random::NextFloat(-1, 1);
	offshoot->m_position.y = Random::NextFloat(-1, 1);
	offshoot->m_position.z = Random::NextFloat(-1, 1);
	offshoot->m_position.Normalize();
	offshoot->m_position *= worldRadius;

	// Randomize orientation (tangent to world surface).
	Vector3f axis = offshoot->m_position.Cross(Vector3f::UP).Normalize();
	float angle = Math::ACos(Vector3f::Normalize(offshoot->m_position).Dot(Vector3f::UP));
	offshoot->m_orientation = Quaternion(axis, angle);
	offshoot->m_orientation.Rotate(offshoot->m_orientation.GetUp(),
		Random::NextFloat() * Math::TWO_PI);

	m_offshoots.push_back(offshoot);

	// Insert the agent into the oct-tree.					TODO: make this not crash
	theSimulation->GetOctTree()->InsertObject(offshoot);

	return offshoot;
}