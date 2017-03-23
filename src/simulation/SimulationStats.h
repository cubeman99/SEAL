#ifndef _SIMULATION_STATS_H_
#define _SIMULATION_STATS_H_

#include "Genome.h"



//-----------------------------------------------------------------------------
// SpeciesStats - various statistics for a single agent species.
//-----------------------------------------------------------------------------
struct SpeciesStats
{
public:
	//-------------------------------------------------------------------------
	// General

	float populationSize;
	float totalEnergy;
	float avgEnergy;
	float avgEnergyUsage;
	float avgFitness;
	float bestFitness;

	//-------------------------------------------------------------------------
	// Output neurons

	float avgMoveAmount; // 0 to 1
	float avgTurnAmount; // -1 to 1
	
	//-------------------------------------------------------------------------
	// Genes

	float avgGeneValue[GenePosition::PHYSIOLOGICAL_GENES_COUNT];

	void Reset()
	{
		memset(avgGeneValue, 0, GenePosition::PHYSIOLOGICAL_GENES_COUNT * sizeof(float));

		avgFitness = 0.0f;
		totalEnergy = 0.0f;
		avgEnergy = 0.0f;
		avgEnergyUsage = 0.0f;
		populationSize = 0;
		avgMoveAmount = 0.0f;
		avgTurnAmount = 0.0f;
		bestFitness = 0.0f;
	}
};


//-----------------------------------------------------------------------------
// SimulationStats - various simulation statistics.
//-----------------------------------------------------------------------------
struct SimulationStats
{
public:

	int simulationAge;
	

	//-------------------------------------------------------------------------
	// Species statistics

	union
	{
		SpeciesStats species[SPECIES_COUNT + 1];

		struct
		{
			SpeciesStats herbivore; // species[0]
			SpeciesStats carnivore; // species[1]
			SpeciesStats combined; // species[2]
		};
	};



public:
	//-------------------------------------------------------------------------

	SimulationStats()
	{
		simulationAge = 0;

		species[SPECIES_HERBIVORE].Reset();
		species[SPECIES_CARNIVORE].Reset();
		species[SPECIES_COUNT].Reset();
	}
};


#endif // _SIMULATION_STATS_H_