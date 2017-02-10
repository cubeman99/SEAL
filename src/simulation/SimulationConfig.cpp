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
	plant.numPlants					= 10;
	plant.numOffshootsPerPlant		= 6;
	plant.maxEnergy					= 100.0f;
	plant.offshootSpawnRadius		= 60;
	plant.growthRate				= 0.05f;
	plant.eatEnergyDepletionRate	= 10.0f;

	agent.radius					= 5.0f;
	agent.initialPredatorCount		= 10;
	agent.initialPreyCount			= 20;
	agent.minPredatorAgents			= 10;
	agent.minPredatorAgents			= 80;
	agent.minPreyAgents				= 20; 
	agent.maxPreyAgents				= 160;
	agent.maxMoveSpeedAtMinStrength	= 0.4f;
	agent.maxMoveSpeedAtMaxStrength	= 0.2f;
	agent.maxTurnSpeedAtMinStrength	= Math::ToRadians<float>(8);
	agent.maxTurnSpeedAtMaxStrength	= Math::ToRadians<float>(5);
	agent.maxEnergyAtMinStrength	= 20.0f;
	agent.maxEnergyAtMaxStrength	= 80.0f;

	energy.energyCostExist			= 0.0f;
	energy.energyCostMove			= 0.0f;
	energy.energyCostTurn			= 0.0f;
	energy.energyCostNeuron			= 0.0f;
	energy.energyCostSynapse		= 0.0f;

	genes.minLifeSpan				= 60 * 60;
	genes.maxLifeSpan				= 60 * 200;
	genes.minStrength				= 0.0f;
	genes.maxStrength				= 1.0f;
	genes.minChildren				= 1;
	genes.maxChildren				= 2;
	genes.minSightDistance			= 20;
	genes.maxSightDistance			= 60;
	genes.minFieldOfView			= Math::ToRadians<float>(10);
	genes.maxFieldOfView			= Math::ToRadians<float>(120);
	genes.minAngleBetweenEyes		= Math::ToRadians<float>(0);
	genes.maxAngleBetweenEyes		= Math::ToRadians<float>(100);
	genes.minSightResolution		= 1;
	genes.maxSightResolution		= 16;
	genes.maxInternalNeurons		= 128; // TODO: tinker

	genes.minBodyColor[0]			= 0.0f;
	genes.maxBodyColor[0]			= 1.0f;
	genes.minBodyColor[1]			= 0.0f;
	genes.maxBodyColor[1]			= 1.0f;
	genes.minBodyColor[2]			= 0.0f;
	genes.maxBodyColor[2]			= 1.0f;
}


