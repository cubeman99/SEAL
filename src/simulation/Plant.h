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
	friend class AgentSystem;

public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::PLANT);

	Plant();
	~Plant();

	void OnSpawn() override;
	void Update(float timeDelta) override;
	Offshoot* SpawnOffshoot();
	void NotifyOffshootDeath(Offshoot* toRemove);

	inline int GetNumOffshoots() const { return (int) m_offshoots.size(); }
	std::vector<Offshoot*>::iterator offshoots_begin() { return m_offshoots.begin(); }
	std::vector<Offshoot*>::iterator offshoots_end() { return m_offshoots.end(); }

private:
	std::vector<Offshoot*> m_offshoots;
};

#endif // _PLANT_H_