#include "Genome.h"
#include <simulation/Simulation.h>
#include <utilities/Random.h>
#include <math/MathLib.h>

Genome::Genome(Simulation* theSimulation, bool randomized)	:
	m_simulation(theSimulation)
{
	if (randomized)
	{
		const SimulationConfig& config = theSimulation->GetConfig();
		RNG& random = theSimulation->GetRandom();

		// Push back random values
		// TODO: is the # of non-input neurons to genes 1 to 1?
		//						 # of physiological genes	           # of internal neurons      # of output neurons
		for (int i = 0; i < GenePosition::NUERON_GENES_BEGIN + config.genes.maxInternalNeurons + 2; ++i)
		{
			m_genes.push_back((unsigned char)(random.NextInt() % 256)); // TODO: Is this giving us 0-255 values?
		}
	}
}

Genome::~Genome()
{
}

Genome* Genome::SpawnChild(Genome* p1, Genome* p2)
{
	// TODO: mutation and crossover
	return p1;
}

float Genome::GeneLerp(float gene, float minValue, float maxValue)
{
	return Math::Lerp(minValue, maxValue, gene);
}

int Genome::GeneLerp(float gene, int minValue, int maxValue)
{
	return (int)(Math::Lerp((float)minValue, (float)maxValue, gene) + 0.5f);
}

float Genome::GetGeneAsFloat(GenePosition index)
{
	return (float)m_genes[(int)index] / 255.0f; // TODO: doublecheck this
}