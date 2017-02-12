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


	inline unsigned int GetNumNeurons() const { return m_numNeurons; }
	inline unsigned int GetNumInputNeurons() const { return m_numInputNeurons; }
	inline unsigned int GetNumOutputNeurons() const { return m_numOutputNeurons; }
	inline unsigned int GetNumInternalNeurons() const { return (m_numNeurons - m_numInputNeurons - m_numOutputNeurons); }
	inline const Neuron& GetNeuron(unsigned int index) const { return m_neurons[index]; }
	inline const Synapse& GetSynapse(unsigned int index) const { return m_synapses[index]; }

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
};


#endif // _BRAIN_H_