#include "SimulationConfig.h"
#include <math/MathLib.h>


SimulationConfig::SimulationConfig()
{
	// Setup some default values for debug purposes.

	world.radius	= 160; // gives a circumference of about 1000
	world.seed		= -1;

	//-------------------------------------------------------------------------
	// Plant

	plant.radius					= 2.5f;
	plant.color[0]					= 0.0f;
	plant.color[1]					= 1.0f;
	plant.color[2]					= 0.0f;
	plant.numPlants					= 12;
	plant.numOffshootsPerPlant		= 4;
	plant.maxEnergy					= 16.0f;
	plant.offshootSpawnRadius		= 80;
	plant.growthRate				= 0.05f;
	plant.eatEnergyDepletionRate	= 5.0f;

	//-------------------------------------------------------------------------
	// Herbivore

	herbivore.population.initialAgents	= 100;
	herbivore.population.minAgents		= 20;
	herbivore.population.maxAgents		= 160;

	herbivore.agent.radiusAtMinStrength			= 4.0f;
	herbivore.agent.radiusAtMaxStrength			= 6.0f;
	herbivore.agent.maxMoveSpeedAtMinStrength	= 0.7f;
	herbivore.agent.maxMoveSpeedAtMaxStrength	= 0.3f;
	herbivore.agent.maxTurnSpeedAtMinStrength	= Math::ToRadians<float>(8);
	herbivore.agent.maxTurnSpeedAtMaxStrength	= Math::ToRadians<float>(5);
	herbivore.agent.maxEnergyAtMinStrength		= 50.0f;
	herbivore.agent.maxEnergyAtMaxStrength		= 100.0f;
	herbivore.agent.minMatingDistance			= 23.0f;
	herbivore.agent.matingDelay					= 60 * 4;
	
	herbivore.energy.energyCostExist		= 0.006f;
	herbivore.energy.energyCostMove			= 0.014f;
	herbivore.energy.energyCostTurn			= 0.014f;
	herbivore.energy.energyCostNeuron		= 0.0f;
	herbivore.energy.energyCostSynapse		= 0.0f;
	
	herbivore.genes.minChildren				= 1;
	herbivore.genes.maxChildren				= 3;
	herbivore.genes.minMutationRate			= 0.02f;	// 1 in 50 genes
	herbivore.genes.maxMutationRate			= 0.1f;		// 1 in 10 genes
	herbivore.genes.minCrossoverPoints		= 1;
	herbivore.genes.maxCrossoverPoints		= 4;
	herbivore.genes.maxInternalNeurons		= 20;		// TODO: tinker
	herbivore.genes.minLifeSpan				= 60 * 120;
	herbivore.genes.maxLifeSpan				= 60 * 200;
	herbivore.genes.minStrength				= 0.0f;
	herbivore.genes.maxStrength				= 1.0f;
	herbivore.genes.minSightDistance		= 20;
	herbivore.genes.maxSightDistance		= 60;
	herbivore.genes.minFieldOfView			= Math::ToRadians<float>(10);
	herbivore.genes.maxFieldOfView			= Math::ToRadians<float>(120);
	herbivore.genes.minAngleBetweenEyes		= Math::ToRadians<float>(0);
	herbivore.genes.maxAngleBetweenEyes		= Math::ToRadians<float>(100);
	herbivore.genes.minSightResolution		= 1;
	herbivore.genes.maxSightResolution		= 6;
	herbivore.genes.minBodyColor[0]			= 0.0f;
	herbivore.genes.maxBodyColor[0]			= 0.0f;
	herbivore.genes.minBodyColor[1]			= 0.0f;
	herbivore.genes.maxBodyColor[1]			= 0.0f;
	herbivore.genes.minBodyColor[2]			= 1.0f; // herbivore = blue for now
	herbivore.genes.maxBodyColor[2]			= 1.0f;
	
	herbivore.brain.numPrebirthCycles		= 10;
	herbivore.brain.sigmoidSlope			= 1.0f;
	herbivore.brain.maxBias					= 1.0f;
	herbivore.brain.initMaxWeight			= 0.5f;
	herbivore.brain.maxWeight				= 1.0f;
	herbivore.brain.weightLearningRate		= 0.08f;
	herbivore.brain.weightDecayRate			= 0.998f;

	//-------------------------------------------------------------------------
	// Carnivore

	// Set carnivore config to herbivore config
	carnivore = herbivore;
	
	carnivore.population.initialAgents	= 15;
	carnivore.population.minAgents		= 15;
	carnivore.population.maxAgents		= 50;
	
	carnivore.agent.matingDelay			= 60 * 16;
	
	carnivore.genes.minLifeSpan			 = 60 * 80;
	carnivore.genes.maxLifeSpan			 = 60 * 160;
	carnivore.genes.minChildren			= 1;
	carnivore.genes.maxChildren			= 2;
	carnivore.genes.minBodyColor[0]		= 1.0f; // carnivore = red for now
	carnivore.genes.maxBodyColor[0]		= 1.0f;
	carnivore.genes.minBodyColor[1]		= 0.0f;
	carnivore.genes.maxBodyColor[1]		= 0.0f;
	carnivore.genes.minBodyColor[2]		= 0.0f;
	carnivore.genes.maxBodyColor[2]		= 0.0f;
	
	carnivore.energy.energyCostExist	= 0.006f;
	carnivore.energy.energyCostMove		= 0.014f;
	carnivore.energy.energyCostTurn		= 0.035f; // increased this to try prevent meat grinders
}