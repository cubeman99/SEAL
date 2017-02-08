#ifndef _SIMULATION_CONFIG_H_
#define _SIMULATION_CONFIG_H_


//-----------------------------------------------------------------------------
// SimulationConfig - contains the values for various simulation parameters
//-----------------------------------------------------------------------------
struct SimulationConfig
{
public:
	SimulationConfig();

	
	//-------------------------------------------------------------------------
	// General
	
	struct
	{
		float	radius;

	} world;

	//-------------------------------------------------------------------------
	// Agents
	
	struct
	{
		int		minPreyAgents;
		int		maxPreyAgents;
		int		minPredatorAgents;
		int		maxPredatorAgents;
		int		initialPreyCount;
		int		initialPredatorCount;

		float	maxEnergyAtMinStrength;
		float	maxEnergyAtMaxStrength;
		float	maxMoveSpeedAtMinStrength;
		float	maxMoveSpeedAtMaxStrength;
		float	maxTurnSpeedAtMinStrength;
		float	maxTurnSpeedAtMaxStrength;

	} agent;

	//-------------------------------------------------------------------------
	// Plants
	
	struct
	{
		int		numPlants;
		int		numOffshootsPerPlant;
		float	maxPlantOffshootEnergy;

	} plant;

	//-------------------------------------------------------------------------
	// Agent gene ranges
	
	struct
	{
		// Physiological genes
		float	minStrength;
		float	maxStrength;
		int		minLifeSpan;
		int		maxLifeSpan;
		float	minBodyColor[3]; // [R, G, B]
		float	maxBodyColor[3]; // [R, G, B]
		int		minChildren;
		int		maxChildren;
		float	minFieldOfView;
		float	maxFieldOfView;
		float	minAngleBetweenEyes;
		float	maxAngleBetweenEyes;
		float	minSightDistance;
		float	maxSightDistance;
		int		minSightResolution;
		int		maxSightResolution;

		// Neurological genes
		int		maxInternalNeurons;

	} genes;

	//-------------------------------------------------------------------------
	// Energy costs
	
	struct
	{
		float	energyCostMove;
		float	energyCostTurn;
		float	energyCostNeuron;
		float	energyCostSynapse;
		float	energyCostExist;

	} energy;

	//-------------------------------------------------------------------------
	// Brain
	
	struct
	{
		int		numPrebirthCycles;
		float	maxBias;
		float	sigmoidSlope;
		float	maxWeight;
		float	initMaxWeight;
		float	weightDecayRate;

	} brain;
};


#endif // _SIMULATION_CONFIG_H_