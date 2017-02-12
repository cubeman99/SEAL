#ifndef _NERVE_H_
#define _NERVE_H_

#include <vector>


enum class NerveType
{
	INPUT,
	OUTPUT,
};


//-----------------------------------------------------------------------------
// Nerve
//-----------------------------------------------------------------------------
class Nerve
{
public:
	Nerve(NerveType type, int firstNeuron, int numNeurons);

	float Get(int neuronIndex = 0) const;
	void Set(int neuronIndex, float activation);
	void Set(float activation);

	int GetNumNeurons() const { return m_numNeurons; }

	void Configure(float** activations);

private:
	NerveType	m_type;
	float**		m_activations;
	int			m_numNeurons;
	int			m_firstNeuron;
};


#endif // _NERVE_H_