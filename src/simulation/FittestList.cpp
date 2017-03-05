#include "FittestList.h"
#include <simulation/Agent.h>
#include <assert.h>


FittestList::FittestList() :
	m_capacity(0),
	m_size(0),
	m_fittest(nullptr)
{
}

FittestList::FittestList(int capacity) :
	m_capacity(capacity),
	m_size(0)
{
	m_fittest = new Fittest[m_capacity];
	for (int i = 0; i < m_capacity; i++)
	{
		m_fittest[i].fitness = 0.0f;
		m_fittest[i].agentId = -1;
		m_fittest[i].genome = nullptr;
	}
}

FittestList::~FittestList()
{
	Clear();
	delete [] m_fittest;
	m_fittest = nullptr;
}

void FittestList::Reset(int capacity)
{
	Clear();
	delete [] m_fittest;

	m_capacity = capacity;
	m_fittest = new Fittest[m_capacity];

	for (int i = 0; i < m_capacity; i++)
	{
		m_fittest[i].fitness = 0.0f;
		m_fittest[i].agentId = -1;
		m_fittest[i].genome = nullptr;
	}
}

int FittestList::GetCapacity() const
{
	return m_capacity;
}

int FittestList::GetSize() const
{
	return m_size;
}

bool FittestList::IsEmpty() const
{
	return (m_size == 0);
}

bool FittestList::IsFull() const
{
	return (m_size == m_capacity);
}

void FittestList::Clear()
{
	for (int i = 0; i < m_size; i++)
	{
		delete m_fittest[i].genome;
		m_fittest[i].genome = nullptr;
	}
	m_size = 0;
}

void FittestList::Update(Agent* agent, float fitness)
{
	if (!IsFull() || fitness > m_fittest[m_size - 1].fitness)
	{
		// Determine the rank of the new agent.
		int rank = 0;
		for (int i = 0; i < m_size; i++)
		{
			if (fitness > m_fittest[i].fitness)
			{
				rank = i;
				break;
			}
		}
		
		// Delete the lowest-ranked genome if were full.
		if (IsFull())
		{
			delete m_fittest[m_size - 1].genome;
			m_fittest[m_size - 1].genome = nullptr;
		}
		else
		{
			m_size++;
		}

		// Shift other agents up (down in rank).
		for (int i = m_size - 1; i > rank; i--)
			m_fittest[i] = m_fittest[i - 1];
		
		// Insert the new genome at its appropriate rank.
		m_fittest[rank].fitness = fitness;
		m_fittest[rank].agentId	= agent->GetId();
		m_fittest[rank].genome = new Genome(*agent->GetGenome());
	}
}

Fittest* FittestList::GetByRank(int rank)
{
	assert(rank >= 0 && rank < m_size);
	return &m_fittest[rank];
}



Genome* FittestList::PickOneRandom(RNG& random)
{
	if (m_size == 0)
		return nullptr;
	int index = random.NextInt(0, m_size);
	return m_fittest[index].genome;
}

Genome* FittestList::PickOneRandomWeighted(RNG& random)
{
	if (m_size == 0)
		return nullptr;

	// Sum up the total fitness of all agents in the list.
	float totalFitness = 0.0f;
	for (int i = 0; i < m_size; ++i)
		m_fittest[i].fitness += totalFitness;

	// If all agents are equally weak, pick any one.
	if (totalFitness == 0.0f)
		return PickOneRandom(random);

	// Otherwise, pick a random agent weighted by fitness.
	float select = random.NextFloat(0.0f, totalFitness);
	float counter = 0.0f;
	for (int i = 0; i < m_size; ++i)
	{
		counter += m_fittest[i].fitness;
		if (select <= counter)
			return m_fittest[i].genome;
	}

	// This shouldn't happen, but handle it anyways.
	return PickOneRandom(random);
}

void FittestList::PickTwoTournamentSelection(RNG& random,
	int tournamentSize, Genome*& outFirst, Genome*& outSecond)
{
	assert(m_size >= 2);
	
	// Pick the first agent.
	int firstIndex = m_size - 1;
	for (int i = 0; i < tournamentSize; ++i)
	{
		int r = random.NextInt(0, m_size);
		if (firstIndex > r)
			firstIndex = r;
	}

	// Pick the second agent.
	int secondIndex = m_size - 1;
	for (int i = 0; i < tournamentSize; ++i)
	{
		int r = (firstIndex + 1 + random.NextInt(0, m_size - 1)) % m_size;
		if (secondIndex > r)
			secondIndex = r;
	}

	outFirst = m_fittest[firstIndex].genome;
	outSecond = m_fittest[secondIndex].genome;
}

