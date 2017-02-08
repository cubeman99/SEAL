#ifndef _OFFSHOOT_H_
#define _OFFSHOOT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"

class Plant;


//-----------------------------------------------------------------------------
// Offshoot - A plant offshoot
//-----------------------------------------------------------------------------
class Offshoot : public SimulationObject
{
	friend class Plant;

public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::OFFSHOOT);

	Offshoot(Plant* plant);
	~Offshoot();
	
	void OnSpawn() override;
	void OnDestroy() override;
	void Update() override;

	// Eat this plant offshoot. Returns the amount of energy eaten.
	float Eat();

private:
	Plant* m_source;
	float m_energy;
	float m_maxEnergy;
	float m_growthRate;
};


#endif // _OFFSHOOT_H_