#include "ParticleSystem.h"

Particle::Particle()
{
	m_type = INVALID_PARTICLE;
	m_lifeTime = 60;
	m_age = 0;
	m_position = Vector3f(0.0f, 0.0f, 0.0f);
	m_velocity;
	m_acceleration;

	m_inUse;
}

ParticleSystem::ParticleSystem()
{

}