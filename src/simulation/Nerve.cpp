#include "Nerve.h"
#include <assert.h>

Nerve::Nerve(NerveType type, int firstNeuron, int numNeurons) :
	m_type(type),
	m_firstNeuron(firstNeuron),
	m_numNeurons(numNeurons),
	m_activations(nullptr)
{
}

float Nerve::Get(int neuronIndex) const
{
	assert(neuronIndex >= 0 && neuronIndex < m_numNeurons);
	return (*m_activations)[m_firstNeuron + neuronIndex];
}

void Nerve::Set(int neuronIndex, float activation)
{
	assert(neuronIndex >= 0 && neuronIndex < m_numNeurons);
	(*m_activations)[m_firstNeuron + neuronIndex] = activation;
}

void Nerve::Set(float activation)
{
	Set(0, activation);
}

void Nerve::Configure(float** activations)
{
	m_activations = activations;
}

