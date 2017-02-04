#include "Offshoot.h"

Offshoot::Offshoot(float maxEnergy)
{
	m_maxEnergy = maxEnergy;
	m_energy = maxEnergy / 2.0f;
	
	m_color.Set(0.3f, 1, 0.1f);
}

Offshoot::~Offshoot()
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

