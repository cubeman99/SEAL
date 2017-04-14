#ifndef _SIMULATION_CONFIG_H_
#define _SIMULATION_CONFIG_H_


//-----------------------------------------------------------------------------
// Species - list of species types
//-----------------------------------------------------------------------------
enum Species
{
	SPECIES_HERBIVORE = 0,
	SPECIES_CARNIVORE = 1,

	SPECIES_COUNT,
};


// Config file parameters explained in "assets/configs/sample_config.txt"


//-----------------------------------------------------------------------------
// SpeciesConfig - configuration parameters for a single agent species.
//-----------------------------------------------------------------------------
struct SpeciesConfig
{
	//-------------------------------------------------------------------------
	// Population sizes

	struct
	{
		int		initialAgents;
		int		minAgents;
		int		maxAgents;

	} population;

	//-------------------------------------------------------------------------
	// Fittest list

	struct
	{
		int		numFittestAgents;
		int		mateTournamentSize;
		float	mateChance;
		float	randomChance;
		float	eliteChance;
	} fittestList;

	//-------------------------------------------------------------------------
	// Agent configuration

	struct
	{
		int		matingDelay;
		float	minMatingDistance;
		float	radiusAtMinStrength;
		float	radiusAtMaxStrength;
		float	maxEnergyAtMinStrength;
		float	maxEnergyAtMaxStrength;
		float	maxMoveSpeedAtMinStrength;
		float	maxMoveSpeedAtMaxStrength;
		float	maxTurnSpeedAtMinStrength;
		float	maxTurnSpeedAtMaxStrength;
		bool	collisions;

	} agent;

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
	// Agent gene ranges
	
	struct
	{
		// Reproduction genes
		float	minMutationRate;	// Chance pre gene that a bit will flip
		float	maxMutationRate; 
		int		minCrossoverPoints;	// The points in which crossover will occur. 
		int		maxCrossoverPoints;	// Apply once to phys genes and again for neurological genes.
		int		minChildren;
		int		maxChildren;

		// Physiological genes
		float	minStrength;
		float	maxStrength;
		int		minLifeSpan;
		int		maxLifeSpan;
		float	minBodyColor[3]; // [R, G, B]
		float	maxBodyColor[3]; // [R, G, B]
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



//-----------------------------------------------------------------------------
// SimulationConfig - Configuration parameters for an entire simulation.
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
		int		seed;	// -1 means random seed.
		int		matingSeasonDuration; // for how many ticks does mating season occur?
		int		offSeasonDuration; // how many ticks from when mating season ends to when it starts again

	} world;

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
	// Agent species

	union
	{
		SpeciesConfig species[SPECIES_COUNT];

		struct
		{
			SpeciesConfig herbivore; // species[0]
			SpeciesConfig carnivore; // species[1]
		};
	};

};


#endif // _SIMULATION_CONFIG_H_