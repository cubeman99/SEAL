#ifndef _AGENT_H_
#define _AGENT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"
#include <simulation/SimulationConfig.h>
#include "Vision.h"
#include "Genome.h"
#include "Brain.h"
#include <math/MathLib.h>

class Offshoot;


//-----------------------------------------------------------------------------
// Agent - A single being of the simulation with its own body and brain.
//-----------------------------------------------------------------------------
class Agent : public SimulationObject
{
public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::AGENT);

	//-------------------------------------------------------------------------
	// Constructors & destructor

	// Adam and Eve constructor
	Agent(Species species = SPECIES_HERBIVORE);
	
	// Natural born constructor
	Agent(Genome* genome, float energy, Species species);

	~Agent();

	//-------------------------------------------------------------------------
	// Simulation object methods

	void OnSpawn() override;
	void OnDestroy() override;
	void Update() override;
	void Read(std::ifstream& fileIn) override;
	void Write(std::ofstream& fileOut) override;

	//-------------------------------------------------------------------------
	// Agent methods

	void UpdateVision();
	void UpdateBrain();
	void SeeObject(SimulationObject* object);
	void EatPlant(Offshoot* plant);
	void Mate(Agent* other);
	void Attack(Agent* other);
	void OnTouchAgent(Agent* other);
	void Die();

	//-------------------------------------------------------------------------
	// Getters
	
	inline Species GetSpecies() const { return m_species; }
	inline int GetAge() const { return m_age; }
	inline float GetEnergy() const { return m_energy; }
	inline float GetHealthEnergy() const { return m_healthEnergy; }
	inline float GetEnergyPercent() const { return (m_energy / m_maxEnergy); }
	inline float GetEnergyUsage() const { return m_energyUsage; }
	inline float GetFitness() const { return m_fitness; }
	inline float GetMoveSpeed() const { return m_moveSpeed; }
	inline float GetTurnSpeed() const { return m_turnSpeed; }
	inline float GetMoveAmount() const { return m_moveAmount; }
	inline float GetTurnAmount() const { return m_turnAmount; }
	inline float GetMaxTurnSpeed() const { return m_maxTurnSpeed; }
	inline float GetMaxMoveSpeed() const { return m_maxMoveSpeed; }
	inline float GetMaxEnergy() const { return m_maxEnergy; }
	inline int GetLifeSpan() const { return m_lifeSpan; }
	inline float GetMutationRate() const { return m_mutationRate; }
	inline int GetDesiredNumChildren() const { return m_desiredNumChildren; }
	inline int GetNumCrossoverPoints() const { return m_numCrossoverPoints; }
	inline float GetStrength() const { return m_strength; }
	inline float GetFieldOfView() const { return m_fieldOfView; }
	inline float GetMaxViewDistance() const { return m_maxViewDistance; }
	inline float GetAngleBetweenEyes() const { return m_angleBetweenEyes; }
	inline unsigned int GetSightResolution(unsigned int channel) const { return m_eyes[0].GetResolution(channel); }
	inline unsigned int GetNumEyes() const { return m_numEyes; }
	inline Retina* GetEye(unsigned int index) { return &m_eyes[index]; }
	inline const Retina* GetEye(unsigned int index) const { return &m_eyes[index]; }
	inline Brain* GetBrain() { return m_brain; }
	inline Genome* GetGenome() { return m_genome; }
	
	//-------------------------------------------------------------------------
	// Setters

	inline void SetEnergy(float energy) { m_energy = Math::Clamp(energy, 0.0f, m_maxEnergy); }
	inline void SetHealthEnergy(float healthEnergy) { m_healthEnergy = Math::Clamp(healthEnergy, 0.0f, m_maxEnergy); }

	//-------------------------------------------------------------------------
	// Debug operations

	inline void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }
	inline void SetTurnSpeed(float turnSpeed) { m_turnSpeed = turnSpeed; }
	inline void SetManualOverride(bool manualOverride) { m_manualOverride = manualOverride; }
	inline bool GetManualOverride() { return m_manualOverride; }


private:

	Genome*			m_genome;
	Brain*			m_brain;

	Species			m_species;

	int				m_age;
	float			m_energy;
	float			m_healthEnergy;
	float			m_fitness;
	float			m_moveAmount;
	float			m_turnAmount;
	float			m_moveSpeed; // Distance units per second.
	float			m_turnSpeed; // Radians per second.
	int				m_mateWaitTime;
	float			m_energyUsage;
	
	// Genome determined
	float			m_strength;
	float			m_maxMoveSpeed;
	float			m_maxTurnSpeed;
	float			m_maxEnergy;
	int				m_lifeSpan;
	int				m_desiredNumChildren;
	float			m_mutationRate;
	int				m_numCrossoverPoints;
	float			m_fieldOfView; // in radians.
	float			m_maxViewDistance;
	float			m_angleBetweenEyes; // in radians.
	
	// Vision
	unsigned int	m_numEyes;
	Retina			m_eyes[2]; // 0 = left eye, 1 = right eye.

	
	// DEBUG: enable/disable manual override. This is for debug
	// purposes, in case we want to control our selected agent manually.
	bool m_manualOverride;
};


#endif // _AGENT_H_