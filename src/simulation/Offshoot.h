#ifndef _OFFSHOOT_H_
#define _OFFSHOOT_H_

#include <math/Quaternion.h>
#include <math/Vector3f.h>
#include "SimulationObject.h"

//-----------------------------------------------------------------------------
// Offshoot - A plant offshoot
//-----------------------------------------------------------------------------
class Offshoot : public SimulationObject
{
	friend class Plant;

public:
	Offshoot(float maxEnergy);
	~Offshoot();

	bool UpdateGrowth(float growth); // returns true if used up

private:
	float m_energy;
	float m_maxEnergy;
};


#endif // _OFFSHOOT_H_