#ifndef _PLANT_H_
#define _PLANT_H_

#include "Offshoot.h"
#include <vector>

class Simulation;

//-----------------------------------------------------------------------------
// Plant - Invisible source unit of vegetation for herbivore agents
//-----------------------------------------------------------------------------
class Plant : public SimulationObject
{
	friend class PlantSystem;

public:
	Plant(Simulation* sim, float maxEnergy);
	~Plant();

	bool Update(float growth); // returns true if it needs to "respawn"
	Offshoot* SpawnOffshoot();

	inline int GetNumOffshoots() const { return (int)m_offshoots.size(); }
	std::vector<Offshoot*>::iterator offshoots_begin() { return m_offshoots.begin(); }
	std::vector<Offshoot*>::iterator offshoots_end() { return m_offshoots.end(); }

private:
	std::vector<Offshoot*> m_offshoots;
	Simulation* m_simulation;
	float m_maxEnergy;
};


#endif // _PLANT_H_