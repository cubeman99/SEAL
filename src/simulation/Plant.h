#ifndef _PLANT_H_
#define _PLANT_H_

#include "Offshoot.h"
#include <vector>

class Simulation;

//-----------------------------------------------------------------------------
// Plant - Invisible source unit of vegetation for herbivore agents. The plant
//         spawns Offshoots within a radius around it.
//-----------------------------------------------------------------------------
class Plant : public SimulationObject
{
	friend class AgentSystem;

public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::PLANT);

	Plant();
	~Plant();

	void OnSpawn() override;
	void Update() override;
	void Read(std::ifstream& fileIn) override;
	void Write(std::ofstream& fileOut) override;

	Offshoot* SpawnOffshoot();
	void NotifyOffshootDeath(Offshoot* toRemove);
	void NotifyOffshootChild(Offshoot* toAdd); // For serialization purposes

	inline int GetNumOffshoots() const { return (int) m_offshoots.size(); }
	std::vector<Offshoot*>::iterator offshoots_begin() { return m_offshoots.begin(); }
	std::vector<Offshoot*>::iterator offshoots_end() { return m_offshoots.end(); }

private:
	std::vector<Offshoot*> m_offshoots;
};

#endif // _PLANT_H_