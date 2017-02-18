#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <simulation/ObjectManager.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationConfig.h>
#include <simulation/SimulationObject.h>
#include <simulation/World.h>
#include <utilities/Random.h>


//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------
class Simulation
{
public:
	Simulation();
	~Simulation();

	void Initialize(const SimulationConfig& config);
	void Tick();
	void NextGeneration();
	Agent* SelectAgent();

	void ReadSimulation(std::ifstream& fileIn);
	void WriteSimulation(std::ofstream& fileOut);

	inline ObjectManager* GetObjectManager() { return &m_objectManager; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return m_objectManager.GetOctTree(); }
	inline RNG& GetRandom() { return m_random; }
	inline const SimulationConfig& GetConfig() { return m_config; }

	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }

private:
	SimulationConfig	m_config;
	unsigned int		m_ageInTicks;
	World				m_world;
	ObjectManager		m_objectManager;
	RNG					m_random;

	unsigned long		m_originalSeed;
	unsigned int		m_generationAge;
	unsigned int		m_generationDuration;
};


#endif // _SIMULATION_H_