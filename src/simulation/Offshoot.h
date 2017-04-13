#ifndef _OFFSHOOT_H_
#define _OFFSHOOT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"

class Plant;


//-----------------------------------------------------------------------------
// Offshoot - The actual visible and edible object that get's spawned by a
//            hidden Plant object.
//-----------------------------------------------------------------------------
class Offshoot : public SimulationObject
{
	friend class Plant;

public:
	DECLARE_SIMULATION_OBJECT(SimulationObjectType::OFFSHOOT);

	Offshoot();
	Offshoot(Plant* plant);
	~Offshoot();
	
	void OnSpawn() override;
	void OnDestroy() override;
	void Update() override;
	void Read(std::ifstream& fileIn) override;
	void Write(std::ofstream& fileOut) override;

	// Eat this plant offshoot. Returns the amount of energy eaten.
	float Eat(float amount);

private:
	Plant* m_source;
	float m_energy;
	float m_maxEnergy;
	float m_growthRate;
};


#endif // _OFFSHOOT_H_