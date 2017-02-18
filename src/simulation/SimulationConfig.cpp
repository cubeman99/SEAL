#include "SimulationConfig.h"
#include <math/MathLib.h>


SimulationConfig::SimulationConfig()
{
	// Setup some default values for debug purposes.

	world.radius					= 160.0f; // gives a circumference of about 1000
	world.seed						= -1;
	plant.radius					= 2.5f;
	plant.color[0]					= 0.0f;
	plant.color[1]					= 1.0f;
	plant.color[2]					= 0.0f;
	plant.numPlants					= 20;
	plant.numOffshootsPerPlant		= 5;
	plant.maxEnergy					= 16.0f;
	plant.offshootSpawnRadius		= 100;
	plant.growthRate				= 0.05f;
	plant.eatEnergyDepletionRate	= 10.0f;

	agent.radius					= 5.0f;
	agent.radiusAtMinStrength		= 4.0f;
	agent.radiusAtMaxStrength		= 6.0f;
	agent.initialPredatorCount		= 10;
	agent.initialPreyCount			= 20;
	agent.minPredatorAgents			= 10;
	agent.minPredatorAgents			= 80;
	agent.minPreyAgents				= 20; 
	agent.maxPreyAgents				= 160;
	agent.maxMoveSpeedAtMinStrength	= 0.7f;
	agent.maxMoveSpeedAtMaxStrength	= 0.3f;
	agent.maxTurnSpeedAtMinStrength	= Math::ToRadians<float>(8);
	agent.maxTurnSpeedAtMaxStrength	= Math::ToRadians<float>(5);
	agent.maxEnergyAtMinStrength	= 50.0f;
	agent.maxEnergyAtMaxStrength	= 100.0f;

	energy.energyCostExist			= 0.005f;
	energy.energyCostMove			= 0.01f;
	energy.energyCostTurn			= 0.01f;
	energy.energyCostNeuron			= 0.0f;
	energy.energyCostSynapse		= 0.0f;

	genes.minChildren				= 1;
	genes.maxChildren				= 4;
	genes.minMutationRate			= 0.001f;	// 1 in 1000 genes mutate. TODO: tinker
	genes.maxMutationRate			= 0.1f;		// 1 in 10 genes mutate. TODO: tinker
	genes.minCrossoverPoints		= 1;
	genes.maxCrossoverPoints		= 4;

	genes.maxInternalNeurons		= 20;		// TODO: tinker

	genes.minLifeSpan				= 60 * 60;
	genes.maxLifeSpan				= 60 * 200;
	genes.minStrength				= 0.0f;
	genes.maxStrength				= 1.0f;
	genes.minSightDistance			= 20;
	genes.maxSightDistance			= 60;
	genes.minFieldOfView			= Math::ToRadians<float>(10);
	genes.maxFieldOfView			= Math::ToRadians<float>(120);
	genes.minAngleBetweenEyes		= Math::ToRadians<float>(0);
	genes.maxAngleBetweenEyes		= Math::ToRadians<float>(100);
	genes.minSightResolution		= 1;
	//genes.minSightResolution		= 6; // For debugging sight
	genes.maxSightResolution		= 6;
	genes.minBodyColor[0]			= 0.0f;
	genes.maxBodyColor[0]			= 1.0f;
	genes.minBodyColor[1]			= 0.0f;
	genes.maxBodyColor[1]			= 1.0f;
	genes.minBodyColor[2]			= 0.0f;
	genes.maxBodyColor[2]			= 1.0f;

	brain.numPrebirthCycles			= 10;
	brain.sigmoidSlope				= 1.0f;
	brain.maxBias					= 1.0f;
	brain.initMaxWeight				= 0.5f;
	brain.maxWeight					= 1.0f;
	brain.weightLearningRate		= 0.08f;
	brain.weightDecayRate			= 0.99f;

	brain.weightLearningRate		= 0.08f;
	brain.weightDecayRate			= 0.998f;
}


