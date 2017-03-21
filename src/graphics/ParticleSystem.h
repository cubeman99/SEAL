#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include <vector>
#include <math/Vector3f.h>
#include <graphics/Color.h>

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
	void Reassign(ParticleType type, Vector3f position);

	inline bool GetInUse() { return m_inUse; }
	inline Color GetColor() { return m_color; }
	inline float GetRadius() { return m_radius; }
	inline Vector3f GetPosition() { return m_position; }

private:
	friend class ParticleSystem;

	ParticleType m_type;
	Color m_color;
	int m_lifeTime;
	int m_age;
	float m_radius;

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
	~ParticleSystem() {};

	void Initialize();
	void CleanUp();

	void Update();
	void AddParticle(ParticleType type, Vector3f position);

	inline std::vector<Particle*> GetParticles() const { return m_particles; }

private:
	std::vector<Particle*> m_particles; // object pool
	Simulation* m_simulation;

	int m_numFreeParticles;

};

#endif //_PARTICLE_SYSTEM_H_