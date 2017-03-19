#include "ParticleSystem.h"

Particle::Particle()
{
	m_inUse = false;
	m_type = INVALID_PARTICLE;
	m_lifeTime = 60;
	m_age = 0;
	m_color = Color::WHITE;

	m_position = Vector3f(0.0f);
	m_velocity = m_position;
	m_acceleration = m_position;
}

bool Particle::Update()
{
	m_velocity += m_acceleration;
	m_position += m_velocity;
	
	++m_age;

	if (m_age >= m_lifeTime)
	{
		m_inUse = false;
	}

	return m_inUse;
}

void Particle::Reassign(ParticleType type, Vector3f position)
{
	m_inUse = true;
	m_age = 0;
	m_type = type;
	m_color = Color::WHITE;

	m_position = position;
	m_velocity = Vector3f(0.0f);
	m_acceleration = m_velocity;

	switch (type)
	{
	case VIOLENCE:
		m_color = Color::RED;
		break;

	case MATING:
		m_color = Color(255, 105, 180);
		break;
	}
}

ParticleSystem::ParticleSystem()
{
}

void ParticleSystem::Init()
{
	m_numFreeParticles = MAX_PARTICLES;

	// Populate object pool
	m_particles.resize(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		m_particles.push_back(Particle());
	}
}

void ParticleSystem::Update()
{
	bool stillInUse;

	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		if (m_particles[i].m_inUse)
		{
			stillInUse = m_particles[i].Update();

			if (!stillInUse)
			{
				m_numFreeParticles++;
			}
		}
	}
}

void ParticleSystem::Draw()
{
	//...
}

void ParticleSystem::AddParticle(ParticleType type, Vector3f position)
{
	if (m_numFreeParticles > 0)
	{
		bool foundOne = false;

		// Search for unused particle
		for (int i = 0; i < MAX_PARTICLES && !foundOne; ++i)
		{
			if (!m_particles[i].m_inUse)
			{
				foundOne = true;
				m_numFreeParticles--;
				m_particles[i].Reassign(type, position);
			}
		}
	}
}