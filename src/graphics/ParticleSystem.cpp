#include "ParticleSystem.h"
#include "simulation\Simulation.h"

Particle::Particle()
{
	m_inUse = false;
	m_type = INVALID_PARTICLE;
	m_lifeTime = 180;
	m_age = 0;
	m_color = Color::WHITE;
	m_growthFactor = 1.0f;

	m_radius = 4.0f;
	m_position = Vector3f(0.0f);
	m_velocity = m_position;
	m_acceleration = m_position;
}

bool Particle::Update()
{
	m_radius *= m_growthFactor;
	m_velocity += m_acceleration;
	m_position += m_velocity;

	++m_age;

	if (m_age >= m_lifeTime)
	{
		m_inUse = false;
	}

	return m_inUse;
}

void Particle::Reassign(ParticleType type, Vector3f position, RNG& random)
{
	m_inUse = true;
	m_age = 0;
	m_type = type;
	m_position = position;

	// Default values
	m_lifeTime = 180;
	m_radius = 4.0f;
	m_color = Color::WHITE;
	m_velocity = position.Normalize() * 0.7f;
	m_acceleration = Vector3f(0.0f);

	switch (type)
	{
	case AGENT_KILLED:
		m_lifeTime = 20;
		m_radius = 10.0f;
		m_growthFactor = 0.87f;
		m_color = Color::RED;
		m_velocity = position.Normalize() * 2.0f;
		m_velocity += 1.8f * Vector3f(random.NextFloatClamped(),
									random.NextFloatClamped(),
									random.NextFloatClamped());
		break;

	case AGENT_MATED:
		m_growthFactor = 0.997f;
		m_color = Color(255, 105, 180);
		m_acceleration = position.Normalize() * -0.008f;
		m_velocity += 0.1f * Vector3f(random.NextFloatClamped(),
									random.NextFloatClamped(),
									random.NextFloatClamped());
		break;
	}
}

ParticleSystem::ParticleSystem(Simulation* theSimulation)
{
	m_simulation = theSimulation;
	m_particleRNG.SeedTime();
}

ParticleSystem::~ParticleSystem()
{
	CleanUp();
}

void ParticleSystem::Initialize()
{
	CleanUp();

	m_numFreeParticles = MAX_PARTICLES;

	// Populate object pool
	m_particles.resize(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		m_particles[i] = new Particle();
	}
}

void ParticleSystem::CleanUp()
{
	for (unsigned int i = 0; i < m_particles.size(); ++i)
	{
		delete m_particles[i];
	}
	m_particles.clear();
}

void ParticleSystem::Update()
{
	bool stillInUse;

	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		if (m_particles[i]->m_inUse)
		{
			stillInUse = m_particles[i]->Update();

			if (!stillInUse)
			{
				m_numFreeParticles++;
			}
		}
	}
}

void ParticleSystem::AddParticle(ParticleType type, Vector3f position)
{
	if (m_numFreeParticles > 0)
	{
		bool foundOne = false;

		// Search for unused particle
		for (int i = 0; i < MAX_PARTICLES && !foundOne; ++i)
		{
			if (!m_particles[i]->m_inUse)
			{
				foundOne = true;
				m_numFreeParticles--;
				m_particles[i]->Reassign(type, position, m_particleRNG);
			}
		}
	}
}