#include "Offshoot.h"
#include <simulation/Plant.h>
#include <math/MathLib.h>


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
	m_radius = 0.010f * scale;
}

float Offshoot::Eat()
{
	float amount = 10.0f;
	float energy = Math::Max(m_energy, amount);

	m_energy -= amount;
	if (m_energy <= 0.0f)
		Destroy();

	return amount;
}
