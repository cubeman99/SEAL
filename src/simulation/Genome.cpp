#include "Genome.h"
#include <math/MathLib.h>
#include <simulation/Brain.h>
#include <simulation/Simulation.h>
#include <utilities/Random.h>


Genome::Genome(Simulation* theSimulation, bool randomized)	:
	m_simulation(theSimulation)
{
	const SimulationConfig& config = theSimulation->GetConfig();
	
	// Initialize the genome size.
	unsigned int numOutputNeurons = 2;
	unsigned int maxInputNeurons = 2 + (config.genes.maxSightResolution * 3 * 2);
	unsigned int maxNeurons = maxInputNeurons + numOutputNeurons + config.genes.maxInternalNeurons;
	unsigned int numGenes = GenePosition::NUERON_GENES_BEGIN +
		maxNeurons + (maxNeurons * (config.genes.maxInternalNeurons + numOutputNeurons));
	m_genes.resize(numGenes);
	
	// Push back random values.
	if (randomized)
	{
		RNG& random = theSimulation->GetRandom();
		for (unsigned int i = 0; i < numGenes; ++i)
			m_genes[i] = (unsigned char) (random.NextInt() % 256);
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


float Genome::GetGeneAsFloat(unsigned int index, float minValue, float maxValue) const
{
	return Math::Lerp(minValue, maxValue, m_genes[index] / 255.0f);
}

int Genome::GetGeneAsInt(unsigned int index, int minValue, int maxValue) const
{
	return (int) (Math::Lerp((float) minValue, (float) maxValue, m_genes[index] / 255.0f) + 0.5f);
}

unsigned int Genome::GetGeneAsInt(unsigned int index, unsigned int minValue, unsigned int maxValue) const
{
	return (unsigned int) (Math::Lerp((float) minValue, (float) maxValue, m_genes[index] / 255.0f) + 0.5f);
}

void Genome::GrowBrain(Brain* brain)
{
	const SimulationConfig& config = m_simulation->GetConfig();
	RNG& random = m_simulation->GetRandom();

	unsigned int resRed = GetGeneAsInt(RESOLUTION_RED,
		config.genes.minSightResolution, config.genes.maxSightResolution);
	unsigned int resGreen = GetGeneAsInt(RESOLUTION_GREEN,
		config.genes.minSightResolution, config.genes.maxSightResolution);
	unsigned int resBlue = GetGeneAsInt(RESOLUTION_BLUE,
		config.genes.minSightResolution, config.genes.maxSightResolution);

	// Count the number of neurons.
	unsigned int numInputNeurons = 2 + ((resRed + resGreen + resBlue) * 2);
	unsigned int numOutputNeurons = 2;
	unsigned int numInternalNeurons = 0;
	
	unsigned int maxInputNeurons = 2 + (config.genes.maxSightResolution * 3 * 2);
	unsigned int maxNeurons = maxInputNeurons + numOutputNeurons + config.genes.maxInternalNeurons;
	unsigned int maxSynapses = (numOutputNeurons + config.genes.maxInternalNeurons) * maxNeurons;

	std::vector<Neuron> neurons;
	std::vector<Synapse> synapses;

	// Parse neuron bias genes.
	for (unsigned int i = 0; i < maxNeurons; ++i)
	{
		Neuron neuron;
		float bias = GetGeneAsFloat(NUERON_GENES_BEGIN + i,
			-config.brain.maxBias, config.brain.maxBias);
		neuron.bias = bias;

		if (i >= maxInputNeurons)
		{
			neuron.synapsesBegin = (i - maxInputNeurons) * maxNeurons;
			neuron.synapsesEnd = neuron.synapsesBegin + maxNeurons;
		}
		else
		{
			neuron.synapsesBegin = 0;
			neuron.synapsesEnd = 0;
		}

		neurons.push_back(neuron);
	}

	// Parse synapse weight genes.
	for (unsigned int i = 0; i < maxSynapses; ++i)
	{
		Synapse synapse;
		synapse.neuronFrom = i % maxNeurons;
		synapse.neuronTo = i / maxNeurons;
		synapse.learningRate = config.brain.weightLearningRate;
		
		// Randomize synapse weight.
		// Positive for excitatory, negative for inhibitory.
		float gene = GetGeneAsFloat(NUERON_GENES_BEGIN + maxNeurons + i, 0.0f, 3.0f);
		if (gene > 2.0f)
			synapse.weight = random.NextFloat(1e-10f, config.brain.initMaxWeight);
		else if (gene < 1.0f)
			synapse.weight = random.NextFloat(-config.brain.initMaxWeight, -1e-10f);
		else
			synapse.weight = 0.0f;

		synapses.push_back(synapse);
	}
	
	// Initialize the brain with the neurons and synapses.
	brain->Initialize(neurons, synapses, 0.1f);
	brain->SetNumInputNeurons(maxInputNeurons);
	brain->SetNumOutputNeurons(numOutputNeurons);

}

