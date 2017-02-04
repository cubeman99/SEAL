#include "Offshoot.h"
#include <simulation/Plant.h>


Offshoot::Offshoot(Plant* plant) :
	m_source(plant)
{
	m_maxEnergy = 100.0f;
	m_energy = m_maxEnergy / 2.0f;
	m_growthRate = 0.2f;
	
	m_isVisible = true;
	m_radius = 0.010f; // TODO: magic number offshoot radius.
	m_color.Set(0.3f, 1, 0.1f);
}

Offshoot::~Offshoot()
{
}

void Offshoot::OnDestroy()
{
	m_source->NotifyOffshootDeath(this);
}

void Offshoot::Update(float timeDelta)
{
	if (m_energy > 0.0f)
	{
		m_energy += m_growthRate * timeDelta;

		if (m_energy > m_maxEnergy)
		{
			m_energy = m_maxEnergy;
		}
	}
	else
	{
		Destroy();
	}
}