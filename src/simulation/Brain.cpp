#include "Brain.h"
#include <math/MathLib.h>


Brain::Brain() :
	m_currNeuronActivations(nullptr),
	m_prevNeuronActivations(nullptr),
	m_neurons(nullptr),
	m_numNeurons(0),
	m_numInputNeurons(0),
	m_numOutputNeurons(0),
	m_synapses(nullptr),
	m_numSynapses(0)
{
}

Brain::~Brain()
{
	delete m_currNeuronActivations;
	m_currNeuronActivations = nullptr;
	delete m_prevNeuronActivations;
	m_prevNeuronActivations = nullptr;
	delete m_neurons;
	m_neurons = nullptr;
	delete m_synapses;
	m_synapses = nullptr;
}

void Brain::Initialize(const std::vector<Neuron>& neurons, const std::vector<Synapse>& synapses, float initialActivation)
{
	Initialize(neurons.size(), synapses.size(), initialActivation);
	
	for (unsigned int i = 0; i < neurons.size(); ++i)
		m_neurons[i] = neurons[i];
	for (unsigned int i = 0; i < synapses.size(); ++i)
		m_synapses[i] = synapses[i];
}

void Brain::Initialize(unsigned int numNeurons, unsigned int numSynapses, float initialActivation)
{
	delete m_currNeuronActivations;
	delete m_prevNeuronActivations;
	delete m_neurons;
	delete m_synapses;

	m_numNeurons = numNeurons;
	m_numSynapses = numSynapses;

	// Allocate neuron and synapse arrays.
	m_synapses = new Synapse[numSynapses];
	m_neurons = new Neuron[numNeurons];
	m_currNeuronActivations = new float[numNeurons];
	m_prevNeuronActivations = new float[numNeurons];
	
	// Setup the initial neuron activation values.
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		m_currNeuronActivations[i] = initialActivation;
		m_prevNeuronActivations[i] = initialActivation;
	}
}

void Brain::ConfigNeuron(unsigned int neuronIndex, float bias, unsigned int synapsesBegin, unsigned int synapsesEnd)
{
	Neuron& neuron = m_neurons[neuronIndex];
	neuron.bias = bias;
	neuron.synapsesBegin = synapsesBegin;
	neuron.synapsesEnd = synapsesEnd;
}

void Brain::ConfigSynapse(unsigned int synapseIndex, float weight, float learningRate, unsigned int neuronFrom, unsigned int neuronTo)
{
	Synapse& synapse = m_synapses[synapseIndex];
	synapse.weight = weight;
	synapse.learningRate = learningRate;
	synapse.neuronFrom = neuronFrom;
	synapse.neuronTo = neuronTo;
}

void Brain::Update()
{
	unsigned int i, k;

	int firstOutputNeuron = m_numInputNeurons;
	float sigmoidSlope = 1.0f;

	//-----------------------------------------------------------------------------
	// Swap the previous and current neuron activation buffers.

	float* tempActivations = m_currNeuronActivations;
	m_currNeuronActivations = m_prevNeuronActivations;
	m_prevNeuronActivations = tempActivations;

	//-----------------------------------------------------------------------------
	// Compute the updated activation values for the internal and output neurons.

	for (i = firstOutputNeuron; i < m_numNeurons; ++i)
	{
		// Add in the bias term.
		float activation = m_neurons[i].bias;

		// Sum up the input activations to this neuron multiplied
		// by their synapse weights.
		for (k = m_neurons[i].synapsesBegin; k < m_neurons[i].synapsesEnd; ++k)
		{
			activation += m_synapses[k].weight *
				m_prevNeuronActivations[m_synapses[k].neuronFrom];
		}

		if (m_neurons[i].synapsesBegin == m_neurons[i].synapsesEnd)
			m_currNeuronActivations[i] = 0.0f;
		else
			// Apply the sigmoid function to the resulting activation sum.
			m_currNeuronActivations[i] = Sigmoid(activation, sigmoidSlope);
	}
	
	//-----------------------------------------------------------------------------
	// Update learning for all synapses.

	for (k = 0; k < m_numSynapses; ++k)
	{
		Synapse& synapse = m_synapses[k];

		// Hebbian learning.
		float efficacy = synapse.weight + synapse.learningRate
			* (m_currNeuronActivations[synapse.neuronTo] - 0.5f)
			* (m_prevNeuronActivations[synapse.neuronFrom] - 0.5f);
				
		if (synapse.learningRate != 0.0f)
		{
			// Gradually decay synapse efficacy.
			if (Math::Abs(efficacy) > (0.5f * m_maxWeight))
			{
				efficacy *= 1.0f - (1.0f - m_decayRate) *
					(Math::Abs(efficacy) - 0.5f * m_maxWeight) / (0.5f * m_maxWeight);
				efficacy = Math::Clamp(efficacy, -m_maxWeight, m_maxWeight);
			}
			else
			{
				// not strictly correct for this to be in an else clause,
				// but if lrate is reasonable, efficacy should never change
				// sign with a new magnitude greater than 0.5 * Brain::config.maxWeight
				if (synapse.learningRate >= 0.0f)  // excitatory
					efficacy = Math::Max(0.0f, efficacy);
				if (synapse.learningRate < 0.0f)  // inhibitory
					efficacy = Math::Min(-1e-10f, efficacy);
			}
		}

		synapse.weight = efficacy;
	}
}


float Brain::Sigmoid(float x, float slope)
{
    return (1.0f / (1.0f + Math::Exp(-x * slope)));
}


