#ifndef _BRAIN_H_
#define _BRAIN_H_

#include <vector>
#include <utilities/Random.h>


//-----------------------------------------------------------------------------
// OutputNeurons
//-----------------------------------------------------------------------------
enum OutputNeurons // TODO: decide if this is necessary or just use an int of numOutputNuerons
{
	MOVE_SPEED = 0,
	TURN_SPEED,

	//----------------

	NUM_OUTPUT_NUERONS
};


//-----------------------------------------------------------------------------
// Neuron
//-----------------------------------------------------------------------------
struct Neuron
{
	float			bias;
	unsigned int	synapsesBegin;
	unsigned int	synapsesEnd;
};


//-----------------------------------------------------------------------------
// Synapse
//-----------------------------------------------------------------------------
struct Synapse
{
	float			weight; // > 0 for excitatory, < 0 for inhibitory
	float			learningRate; // > 0 for excitatory, < 0 for inhibitory
	unsigned int	neuronFrom;
	unsigned int	neuronTo;
};


//-----------------------------------------------------------------------------
// Brain
//-----------------------------------------------------------------------------
class Brain
{
public:
	Brain();
	~Brain();

	// Initialization
	void Initialize(const std::vector<Neuron>& neurons, const std::vector<Synapse>& synapses, float initialActivation);
	void Initialize(unsigned int numNeurons, unsigned int numSynapses, float initialActivation);
	void ConfigNeuron(unsigned int neuronIndex, float bias, unsigned int synapsesBegin, unsigned int synapsesEnd);
	void ConfigSynapse(unsigned int synapseIndex, float weight, float learningRate, unsigned int neuronFrom, unsigned int neuronTo);
	
	// Simulation
	void PreBirth(unsigned int numCycles, RNG& random);
	void Update();

	// Getters
	inline unsigned int GetNumNeurons() const { return m_numNeurons; }
	inline unsigned int GetNumInputNeurons() const { return m_numInputNeurons; }
	inline unsigned int GetNumOutputNeurons() const { return m_numOutputNeurons; }
	inline unsigned int GetNumInternalNeurons() const { return (m_numNeurons - m_numInputNeurons - m_numOutputNeurons); }
	inline unsigned int GetNumSynapses() const { return m_numSynapses; }
	inline const Neuron& GetNeuron(unsigned int index) const { return m_neurons[index]; }
	inline const Synapse& GetSynapse(unsigned int index) const { return m_synapses[index]; }
	inline float GetNeuronActivation(unsigned int index) const { return m_currNeuronActivations[index]; }
	inline float GetPrevNeuronActivation(unsigned int index) const { return m_prevNeuronActivations[index]; }

	// Setters
	inline void SetNumInputNeurons(unsigned int numInputNeurons) { m_numInputNeurons = numInputNeurons; }
	inline void SetNumOutputNeurons(unsigned int numOutputNeurons) { m_numOutputNeurons = numOutputNeurons; }
	inline void SetDecayRate(float decayRate) { m_decayRate = decayRate; }
	inline void SetMaxWeight(float maxWeight) { m_maxWeight = maxWeight; }
	inline void SetNeuronActivation(unsigned int index, float activation) { m_currNeuronActivations[index] = activation; }

private:
	// Static functions
	static float Sigmoid(float x, float slope);

private:
	
	Neuron*			m_neurons;
	float*			m_currNeuronActivations;
	float*			m_prevNeuronActivations; // Activation values for each neuron from previous tick.
	unsigned int	m_numNeurons;
	unsigned int	m_numInputNeurons;
	unsigned int	m_numOutputNeurons;

	Synapse*		m_synapses;
	unsigned int	m_numSynapses;

	float			m_decayRate;
	float			m_maxWeight;
};


#endif // _BRAIN_H_