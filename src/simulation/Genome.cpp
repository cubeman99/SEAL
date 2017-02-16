#include "Genome.h"
#include <math/MathLib.h>
#include <simulation/Brain.h>
#include <simulation/Simulation.h>
#include <utilities/Random.h>


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Genome::Genome(Simulation* theSimulation, bool randomized)	:
	m_simulation(theSimulation)
{
	const SimulationConfig& config = theSimulation->GetConfig();
	
	// Determine the genome size.
	unsigned int numOutputNeurons = 2;
	unsigned int maxInputNeurons = 2 + (config.genes.maxSightResolution * 3 * 2); // TODO: Updates and comment. Remember the mating season input?
	unsigned int maxNeurons = maxInputNeurons + numOutputNeurons + config.genes.maxInternalNeurons;
	unsigned int numGenes = GenePosition::NUERON_GENES_BEGIN +
		maxNeurons + (maxNeurons * (config.genes.maxInternalNeurons + numOutputNeurons));
	m_genes.resize(numGenes);
	
	// Randomize all genes.
	if (randomized)
		Randomize();
}

Genome::~Genome()
{
}

Genome* Genome::SpawnChild(Genome* p1, Genome* p2, Simulation* theSimulation)
{
	const SimulationConfig& config = theSimulation->GetConfig();
	RNG& random = theSimulation->GetRandom();

	// Given to the Agent class of the new agent to destroy when needed.
	Genome* child = new Genome(theSimulation, false);
	Genome* currentParent = p1;

	// TODO: Parents have different mutation and crossover rates, how best to reconcile?
	// For now, going to average them because that seems most fair.

	// Get average mutation and crossover rates
	float p1MutationRate = p1->GetGeneAsFloat(GenePosition::MUTATION_RATE,
		config.genes.minMutationRate,
		config.genes.maxMutationRate);
	float p2MutationRate = p2->GetGeneAsFloat(GenePosition::MUTATION_RATE,
		config.genes.minMutationRate,
		config.genes.maxMutationRate);
	float averageMutationRate = (p1MutationRate + p2MutationRate) / 2.0f;

	float p1CrossoverRate = p1->GetGeneAsFloat(GenePosition::CROSSOVER_RATE,
		config.genes.minCrossoverRate,
		config.genes.maxCrossoverRate);
	float p2CrossoverRate = p2->GetGeneAsFloat(GenePosition::CROSSOVER_RATE,
		config.genes.minCrossoverRate,
		config.genes.maxCrossoverRate);
	float averageCrossoverRate = (p1CrossoverRate + p2CrossoverRate) / 2.0f;

	// TODO: This loop assumes the size of a genome is always maximum, is that correct?

	for (unsigned int i = 0; i < p1->m_genes.size(); ++i)
	{
		// Check for crossover
		if (random.NextFloat() < averageCrossoverRate)
		{
			if (currentParent == p1)
			{
				currentParent = p2;
			}
			else
			{
				currentParent = p1;
			}
		}

		// Transcribe gene
		child->m_genes[i] = currentParent->m_genes[i];

		// Check for mutation
		if (random.NextFloat() <= averageMutationRate)
		{
			// XOR the gene with an empty byte that has a 1 randomly placed inside,
			// effectively flipping a random bit. TODO: is this working correctly?
			child->m_genes[i] ^= (1u << random.NextInt() % 8);
		}
	}

	return child;
}

//-----------------------------------------------------------------------------
// Genome operations
//-----------------------------------------------------------------------------

void Genome::Randomize()
{
	RNG& random = m_simulation->GetRandom();
	for (unsigned int i = 0; i < m_genes.size(); ++i)
		m_genes[i] = (unsigned char) (random.NextInt() % 256);
}

//-----------------------------------------------------------------------------
// Gene access
//-----------------------------------------------------------------------------

float Genome::GetGeneAsFloat(unsigned int index) const
{
	return (float) m_genes[(int) index] / 255.0f;
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


//-----------------------------------------------------------------------------
// Neurogenetics
//-----------------------------------------------------------------------------

void Genome::GrowBrain(Brain* brain)
{
	const SimulationConfig& config = m_simulation->GetConfig();
	RNG& random = m_simulation->GetRandom();

	//-------------------------------------------------------------------------
	// Count up the input and output neurons.

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

	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	// Parse synapse weight genes.

	for (unsigned int i = 0; i < maxSynapses; ++i)
	{
		Synapse synapse;
		synapse.neuronFrom = i % maxNeurons;
		synapse.neuronTo = (i / maxNeurons) + maxInputNeurons;

		// Randomize synapse weight.
		// Positive for excitatory, negative for inhibitory.
		float gene = GetGeneAsFloat(NUERON_GENES_BEGIN + maxNeurons + i, 0.0f, 1.0f);
		if (gene > 0.8f)
		{
			synapse.weight = random.NextFloat(1e-10f, config.brain.initMaxWeight);
			synapse.learningRate = config.brain.weightLearningRate;
		}
		else if (gene < 0.2f)
		{
			synapse.weight = random.NextFloat(-config.brain.initMaxWeight, -1e-10f);
			synapse.learningRate = -config.brain.weightLearningRate;
		}
		else
		{
			synapse.weight = 0.0f;
			synapse.learningRate = 0.0f;
		}
		
		if (synapse.neuronFrom == synapse.neuronTo)
		{
			synapse.weight = 0.0f;
			synapse.learningRate = 0.0f;
		}

		synapses.push_back(synapse);
	}

	//-------------------------------------------------------------------------
	// Initialize the brain with the neurons and synapses.

	brain->Initialize(neurons, synapses, 0.1f);
	brain->SetNumInputNeurons(maxInputNeurons);
	brain->SetNumOutputNeurons(numOutputNeurons);
	brain->SetMaxWeight(config.brain.maxWeight);
	brain->SetDecayRate(config.brain.weightDecayRate);

	// TODO: prune the network of redundencies.
	// If an input neuron is not used (vision neuron), then remove it.
	// If an internal neuron has no incoming connections, then remove it.
	// If an internal neuron has no outgoing connections, then remove it.
}

