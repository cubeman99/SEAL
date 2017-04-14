#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include <vector>
#include <math/Vector3f.h>
#include <graphics/Color.h>
#include "utilities/Random.h"

const int MAX_PARTICLES = 666;

enum ParticleType
{
	INVALID_PARTICLE = 0,
	AGENT_KILLED,
	AGENT_MATED,

	NUM_PARTICLE_TYPES
};

class Particle
{
public:
	Particle();
	~Particle() {};

	bool Update();
	void Reassign(ParticleType type, Vector3f position, RNG& random);

	inline ParticleType GetType() const { return m_type; }
	inline bool GetInUse() const { return m_inUse; }
	inline Color GetColor() const { return m_color; }
	inline float GetRadius() const { return m_radius; }
	inline Vector3f GetPosition() const { return m_position; }

private:
	friend class ParticleSystem;

	ParticleType m_type;
	Color m_color;
	int m_lifeTime;
	int m_age;
	float m_radius;
	float m_growthFactor;

	Vector3f m_position;
	Vector3f m_velocity;
	Vector3f m_acceleration;

	bool m_inUse;

};

class Simulation;

class ParticleSystem
{
public:
	ParticleSystem(Simulation* theSimulation);
	~ParticleSystem();

	void Initialize();
	void CleanUp();

	void Update();
	void AddParticle(ParticleType type, Vector3f position);

	inline const std::vector<Particle*>& GetParticles() const { return m_particles; }

private:
	std::vector<Particle*> m_particles; // object pool
	Simulation* m_simulation;
	RNG m_particleRNG;					// Shouldn't affect the RNG of simulation timelines

	int m_numFreeParticles;

};

#endif //_PARTICLE_SYSTEM_H_