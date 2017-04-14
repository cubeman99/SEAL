#ifndef _FITTEST_LIST_H_
#define _FITTEST_LIST_H_

#include <simulation/Genome.h>
#include <vector>

class Agent;


// Fittest List and steady state GA idea modified from Polyworld:
// https://github.com/polyworld/polyworld


//-----------------------------------------------------------------------------
// Fittest - A single entry in the fittest list.
//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
// FittestList - Used to store a list of the most fit genomes.
//-----------------------------------------------------------------------------
class FittestList
{
public:
	//-------------------------------------------------------------------------
	// Constructors & destructor

	FittestList();
	FittestList(int capacity);
	~FittestList();


	//-------------------------------------------------------------------------
	// Getters

	int GetCapacity() const;
	
	int GetSize() const;

	bool IsEmpty() const;

	// Is the list full (does size equal capacity?)
	bool IsFull() const;

	// Get the fittest agent entry by rank, where rank 0 is the fittest.
	Fittest* GetByRank(int rank);


	//-------------------------------------------------------------------------
	// List modification

	// Clear the list.
	void Clear();
	
	// Clear and reset the list with the given capacity.
	void Reset(int capacity);

	// Update the list with the given agent and fitness.
	void Update(Agent* agent, float fitness);


	//-------------------------------------------------------------------------
	// Fittest selection

	// Pick a random genome.
	Genome* PickOneRandom(RNG& random);

	// Pick a random genome weighted by fitness.
	Genome* PickOneRandomWeighted(RNG& random);

	// Pick two random genomes using tournament selection.
	void PickTwoTournamentSelection(RNG& random, int tournamentSize,
		Genome*& outFirst, Genome*& outSecond);


private:
	int			m_capacity;	// Maximum fittest genomes to store in the list.
	int			m_size;		// Current number of genomes in the list.
	Fittest*	m_fittest;	// Array of fittest genome entries.
};


#endif // _FITTEST_LIST_H_