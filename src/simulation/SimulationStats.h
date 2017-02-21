#ifndef _SIMULATION_STATS_H_
#define _SIMULATION_STATS_H_

#include "Genome.h"


struct SimulationStats
{
public:
	//-------------------------------------------------------------------------
	// Genes

	float avgGeneValues[GenePosition::PHYSIOLOGICAL_GENES_COUNT];

	//float avgViewDistance;
	//float avgFieldOfView;
	//float avgAngleBetweenEyes;
	//float avgResolutionRed;
	//float avgResolutionGreen;
	//float avgResolutionBlue;
	//float avgMutationRate;
	//float avgNumCrossoverPoints;
	//float avgChildCount;
	//float avgLifeSpan;
	//float avgStrength;
	//float avgColorRed;
	//float avgColorGreen;
	//float avgColorBlue;

	//-------------------------------------------------------------------------
	// Performance

	int simulationAge;

	float avgFitness;
	//float bestFitness;
	float totalEnergy;
	float populationSize;
	float avgEnergy;
	//float avgEnergyUsage;


public:
	//-------------------------------------------------------------------------

	SimulationStats()
	{
		memset(avgGeneValues, 0, GenePosition::PHYSIOLOGICAL_GENES_COUNT * sizeof(float));
		avgFitness = 0.0f;
		totalEnergy = 0.0f;
		avgEnergy = 0.0f;
		populationSize = 0;
		simulationAge = 0;
	}
};


#endif // _SIMULATION_STATS_H_