#ifndef _PLANT_SYSTEM_H_
#define _PLANT_SYSTEM_H_

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


	Simulation* m_simulation;
};


#endif // _PLANT_SYSTEM_H_