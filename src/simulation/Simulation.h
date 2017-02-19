#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <simulation/ObjectManager.h>
#include <simulation/OctTree.h>
#include <simulation/SimulationConfig.h>
#include <simulation/SimulationObject.h>
#include <simulation/SimulationStats.h>
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

	void UpdateStatistics();

	bool SaveTimeline(std::string fileName);
	bool LoadTimeline(std::string fileName);
	void ReadSimulation(std::ifstream& fileIn);
	void WriteSimulation(std::ofstream& fileOut);

	inline ObjectManager* GetObjectManager() { return &m_objectManager; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return m_objectManager.GetOctTree(); }
	inline RNG& GetRandom() { return m_random; }
	inline const SimulationConfig& GetConfig() { return m_config; }
	inline const SimulationStats& GetStatistics() { return m_statistics; }

	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }

	inline unsigned int GetGeneration() const { return m_generationIndex; }

private:
	SimulationConfig	m_config;
	unsigned int		m_ageInTicks;
	World				m_world;
	ObjectManager		m_objectManager;
	RNG					m_random;

	SimulationStats		m_statistics;

	unsigned long		m_originalSeed;
	unsigned int		m_generationAge;
	unsigned int		m_generationDuration;
	unsigned int		m_generationIndex;

public:
	std::vector<SimulationStats> m_generationStats;
};


#endif // _SIMULATION_H_