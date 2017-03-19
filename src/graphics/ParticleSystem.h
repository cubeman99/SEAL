#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include <vector>

#include <math/Vector3f.h>
#include <graphics/Color.h>

const int MAX_PARTICLES = 666;

enum ParticleType
{
	INVALID_PARTICLE = 0,
	VIOLENCE,
	MATING,

	NUM_PARTICLE_TYPES
};

class Particle
{
public:
	Particle();
	~Particle() {};

	bool Update();
	void Reassign(ParticleType type, Vector3f position);

private:
	friend class ParticleSystem;

	ParticleType m_type;
	int m_lifeTime;
	int m_age;
	Color m_color;

	Vector3f m_position;
	Vector3f m_velocity;
	Vector3f m_acceleration;

	bool m_inUse;

};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem() {};

	void Update();
	void Init();
	void CleanUp();

	void AddParticle(ParticleType type, Vector3f position);

private:
	std::vector<Particle*> m_particles; // object pool

	int m_numFreeParticles;

};

#endif //_PARTICLE_SYSTEM_H_