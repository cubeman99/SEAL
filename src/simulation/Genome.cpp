#include "Genome.h"
#include <math/MathLib.h>
#include <simulation/Brain.h>
#include <simulation/Simulation.h>
#include <utilities/Random.h>


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Genome::Genome(const Genome& copy)
{
	m_genes = copy.m_genes;
}

Genome::Genome(const SpeciesConfig& config)
{	
	m_genes.resize(DetermineGenomeSize(config));
}

Genome::~Genome()
{
}


//-----------------------------------------------------------------------------
// Genome operations
//-----------------------------------------------------------------------------

void Genome::Randomize(RNG& random)
{
	for (unsigned int i = 0; i < m_genes.size(); ++i)
		m_genes[i] = (unsigned char) (random.NextInt() % 256);
}

int Genome::DetermineGenomeSize(const SpeciesConfig& config)
{
	unsigned int numOutputNeurons = 2;
	unsigned int maxInputNeurons = NUM_NON_SIGHT_INPUTS + (config.genes.maxSightResolution * 3 * 2);
	unsigned int maxNeurons = maxInputNeurons + numOutputNeurons + config.genes.maxInternalNeurons;
	return GenePosition::NUERON_GENES_BEGIN +
		maxNeurons + (maxNeurons * (config.genes.maxInternalNeurons + numOutputNeurons));
}

Genome* Genome::SpawnChild(Genome* p1, Genome* p2, const SpeciesConfig& config, RNG& random)
{
	// Given to the Agent class of the new agent to destroy when needed.
	Genome* child = new Genome(config);
	Genome* currentParent = p1;
	const int GENOME_SIZE = (int)p1->m_genes.size();

	// Get average mutation and crossover data from parents
	float p1MutationRate = p1->GetGeneAsFloat(GenePosition::MUTATION_RATE,
		config.genes.minMutationRate,
		config.genes.maxMutationRate);
	float p2MutationRate = p2->GetGeneAsFloat(GenePosition::MUTATION_RATE,
		config.genes.minMutationRate,
		config.genes.maxMutationRate);
	float averageMutationRate = (p1MutationRate + p2MutationRate) / 2.0f;

	int p1CrossoverPoints = p1->GetGeneAsInt(GenePosition::CROSSOVER_POINTS,
		config.genes.minCrossoverPoints,
		config.genes.maxCrossoverPoints);
	int p2CrossoverPoints = p2->GetGeneAsInt(GenePosition::CROSSOVER_POINTS,
		config.genes.minCrossoverPoints,
		config.genes.maxCrossoverPoints);
	int averageCrossoverPoints = (p1CrossoverPoints + p2CrossoverPoints) / 2;

	// If the average was between integers, half chance to round up instead
	if ((p1CrossoverPoints + p2CrossoverPoints) % 2 == 1 && random.NextFloat() > 0.5f)
	{
		++averageCrossoverPoints;
	}

	// Assign crossover points. (They apply once to physiological
	// genes and then again to Neurological genes).
	std::vector<int> crossoverPoints;
	int candidatePoint;
	bool onNeurologicals = false;
	bool foundDuplicate = false;
	int i;
	int j;

	for (i = averageCrossoverPoints; i > 0; --i)
	{
		foundDuplicate = false;

		if (onNeurologicals)
		{
			candidatePoint = (random.NextInt() % (GENOME_SIZE - GenePosition::NUERON_GENES_BEGIN)) 
				+ GenePosition::NUERON_GENES_BEGIN;
		}
		else
		{
			candidatePoint = random.NextInt() % GenePosition::NUERON_GENES_BEGIN;
		}

		// Find ordered spot for candidate (Largest to smallest).
		// This way there are much fewer checks required of the crossover list.
		for (j = 0; j < (int)crossoverPoints.size() && candidatePoint <= crossoverPoints[j]; ++j)
		{
			if (candidatePoint == crossoverPoints[j])
			{
				foundDuplicate = true;
			}
		}

		if (!foundDuplicate)
		{
			crossoverPoints.insert(crossoverPoints.begin() + j, candidatePoint);

			// Done with physiological genes? Now for the neurological
			if (i == 1 && !onNeurologicals)
			{
				i = averageCrossoverPoints + 1; // Acounting for the next --i;
				onNeurologicals = true;
			}
		}
		else
		{
			// Keep trying
			++i;
		}
	}

	// Transcribe and perform genetic algorithms
	for (int i = 0; i < GENOME_SIZE; ++i)
	{
		// Check for crossover
		if (crossoverPoints.size() > 0 && i == crossoverPoints[crossoverPoints.size() - 1])
		{
			// Remove lowest crossover index
			crossoverPoints.pop_back();

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
			// effectively flipping a random bit.
			child->m_genes[i] ^= (1u << random.NextInt() % 8);
		}
	}

	return child;
}


//-----------------------------------------------------------------------------
// Gene access
//-----------------------------------------------------------------------------

float Genome::GetGeneAsFloat(unsigned int index) const
{
	return ((float) m_genes[(int) index]) / 255.0f;
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

void Genome::GrowBrain(Brain* brain, const SpeciesConfig& config, RNG& random)
{
	// Algorithm partially based on Polyworld:
	// https://github.com/polyworld/polyworld

	//-------------------------------------------------------------------------
	// Count up the input and output neurons.

	unsigned int resRed = GetGeneAsInt(RESOLUTION_RED,
		config.genes.minSightResolution, config.genes.maxSightResolution);
	unsigned int resGreen = GetGeneAsInt(RESOLUTION_GREEN,
		config.genes.minSightResolution, config.genes.maxSightResolution);
	unsigned int resBlue = GetGeneAsInt(RESOLUTION_BLUE,
		config.genes.minSightResolution, config.genes.maxSightResolution);

	// Count the number of neurons.
	unsigned int numOutputNeurons = 2;
	unsigned int maxInputNeurons = NUM_NON_SIGHT_INPUTS + (config.genes.maxSightResolution * 3 * 2);
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
