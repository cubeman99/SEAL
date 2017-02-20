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

	//-------------------------------------------------------------------------
	// Getters

	inline ObjectManager* GetObjectManager() { return &m_objectManager; }
	inline World* GetWorld() { return &m_world; }
	inline OctTree* GetOctTree() { return m_objectManager.GetOctTree(); }
	inline RNG& GetRandom() { return m_random; }

	inline const SimulationConfig& GetConfig() const { return m_config; }
	inline const SimulationStats& GetStatistics() const { return m_statistics; }
	inline unsigned int GetAgeInTicks() const { return m_ageInTicks; }
	inline unsigned int GetGeneration() const { return m_generationIndex; }


	//-------------------------------------------------------------------------
	// Initialization and update

	// Begin a new simulation using the given configuration.
	void Initialize(const SimulationConfig& config);

	// Update the simulation for one tick.
	void Tick();

	
	//-------------------------------------------------------------------------
	// Saving & loading

	// Read simulation from file.
	void ReadSimulation(std::ifstream& fileIn);

	// Write simulation data to file.
	void WriteSimulation(std::ofstream& fileOut);


private:
	//-------------------------------------------------------------------------
	// Internal update mechanics

	// Update the statistics
	void UpdateStatistics();

	// Advance to the next generation
	void NextGeneration();

	// Randomly select an agent from the population with chances
	// weighted by fitness.
	Agent* SelectAgent();

	// Create an offspring agent from two parent agents.
	Agent* CreateOffspring(Agent* mommy, Agent* daddy);


private:
	SimulationConfig	m_config;
	World				m_world;
	ObjectManager		m_objectManager;
	RNG					m_random;
	SimulationStats		m_statistics;
	unsigned long		m_originalSeed;
	unsigned int		m_ageInTicks;
	unsigned int		m_generationIndex;
	unsigned int		m_generationAge;
	unsigned int		m_generationDuration; // TODO: move into config.

	// TODO: mating season values here??

public:
	std::vector<SimulationStats> m_generationStats;
};


#endif // _SIMULATION_H_