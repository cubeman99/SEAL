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
		int		seed; // -1 means random seed.

	} world;

	//-------------------------------------------------------------------------
	// Agents
	
	struct
	{
		float	radius;
		int		minPreyAgents;
		int		maxPreyAgents;
		int		minPredatorAgents;
		int		maxPredatorAgents;
		int		initialPreyCount;
		int		initialPredatorCount;

		float	radiusAtMinStrength;
		float	radiusAtMaxStrength;
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
		float	radius;
		float	color[3];
		int		numPlants;
		int		numOffshootsPerPlant;
		float	maxEnergy;
		float	offshootSpawnRadius;
		float	growthRate; // energy per tick
		float	eatEnergyDepletionRate;

	} plant;

	//-------------------------------------------------------------------------
	// Agent gene ranges
	
	struct
	{
		int		numCrossoverPoints;

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
		float	energyCostExist; // energy per tick
		float	energyCostMove; // energy per distance per tick.
		float	energyCostTurn; // energy per radian per tick.
		float	energyCostNeuron;
		float	energyCostSynapse;

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
		float	weightLearningRate;
		float	weightDecayRate;

	} brain;
};


#endif // _SIMULATION_CONFIG_H_