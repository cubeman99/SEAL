#include "Offshoot.h"
#include <simulation/Plant.h>


Offshoot::Offshoot(Plant* plant, float maxEnergy) :
	m_source(plant),
	m_maxEnergy(maxEnergy)
{
	m_energy = maxEnergy / 2.0f;
	
	m_isVisible = true;
	m_radius = 0.010f; // TODO: magic number offshoot radius.
	m_color.Set(0.3f, 1, 0.1f);
}

Offshoot::~Offshoot()
{
}

void Offshoot::OnDestroy()
{
	// TODO: Notify the plant source ??
}

void Offshoot::Update(float timeDelta)
{
	
}

bool Offshoot::UpdateGrowth(float growth)
{
	if (m_energy > 0)
	{
		m_energy += growth;

		if (m_energy > m_maxEnergy)
		{
			m_energy = m_maxEnergy;
		}

		return false; // not dead
	}
	else
	{
		return true; // used up
	}
}

