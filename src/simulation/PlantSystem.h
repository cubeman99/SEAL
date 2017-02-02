#ifndef _PLANT_SYSTEM_H_
#define _PLANT_SYSTEM_H_

#include "Plant.h"
#include <vector>

class Simulation;

//-----------------------------------------------------------------------------
// PlantSystem - Manages the Plant and PlantSource objects within simulation
//-----------------------------------------------------------------------------
class PlantSystem
{
public:
	PlantSystem(Simulation* simulation, float growthRate, float maxEnergy);
	~PlantSystem();

	Plant* SpawnPlant();
	void RemoveAllPlants();
	void UpdatePlants(float timeDelta);

	inline int GetNumPlants() const { return (int) m_plants.size(); }

	std::vector<Plant*>::iterator plants_begin() { return m_plants.begin(); }
	std::vector<Plant*>::iterator plants_end() { return m_plants.end(); }

private:
	std::vector<Plant*> m_plants;
	Simulation* m_simulation;

	float m_growthRate;
	float m_maxEnergy;
};


#endif // _PLANT_SYSTEM_H_