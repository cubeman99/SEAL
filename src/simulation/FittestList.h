#ifndef _FITTEST_LIST_H_
#define _FITTEST_LIST_H_

#include <simulation/Genome.h>
#include <vector>

class Agent;


struct Fittest
{
	int		agentId;
	float	fitness;
	Genome*	genome;

	Fittest() :
		fitness(0.0f),
		agentId(-1),
		genome(nullptr)
	{
	}
};


class FittestList
{
public:
	FittestList();
	FittestList(int capacity);
	~FittestList();

	void Reset(int capacity);

	int GetCapacity() const;
	int GetSize() const;
	bool IsFull() const;
	void Clear();

	void Update(Agent* agent, float fitness);

	Fittest* GetByRank(int rank);

	void PickTwoTournamentSelection(int tournamentSize, Genome*& outFirst, Genome*& outSecond);

private:
	int			m_capacity;
	int			m_size;
	Fittest*	m_fittest;
};


#endif // _FITTEST_LIST_H_