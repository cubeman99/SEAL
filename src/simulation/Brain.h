#ifndef _BRAIN_H_
#define _BRAIN_H_


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
	float			learningRate;
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

	void Initialize(unsigned int numNeurons, unsigned int numSynapses, float initialActivation);
	void ConfigNeuron(unsigned int neuronIndex, float bias, unsigned int synapsesBegin, unsigned int synapsesEnd);
	void ConfigSynapse(unsigned int synapseIndex, float weight, float learningRate, unsigned int neuronFrom, unsigned int neuronTo);

	void Update();


private:
	static float Sigmoid(float x, float slope);

private:
	
	unsigned int	m_numInputNeurons;
	unsigned int	m_numOutputNeurons;
	Neuron*			m_neurons;
	unsigned int	m_numNeurons;
	Synapse*		m_synapses;
	unsigned int	m_numSynapses;
	float*			m_prevNeuronActivations; // Activation values for each neuron.
	float*			m_currNeuronActivations;
	//float*			m_neuronActivations[2]; // Neuron activation values for the current [0] and previous [1] frames.
};


#endif // _BRAIN_H_