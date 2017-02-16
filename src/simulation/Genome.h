#ifndef _GENOME_H_
#define _GENOME_H_

#include <vector>

class Simulation;
class Brain;


enum GenePosition
{
	// Vision
	VIEW_DISTANCE = 0,
	FIELD_OF_VIEW,
	ANGLE_BETWEEN_EYES,
	RESOLUTION_RED,
	RESOLUTION_GREEN,
	RESOLUTION_BLUE,

	// Reproduction
	MUTATION_RATE,
	CROSSOVER_RATE,
	CHILD_COUNT,

	// Physiological
	LIFE_SPAN,
	STRENGTH,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,

	// First index of nueron genes, also count of non-nueron genes
	NUERON_GENES_BEGIN
};


class Genome
{
public:
	//-------------------------------------------------------------------------
	// Constructor & destructor

	Genome(Simulation* theSimulation, bool randomized);
	~Genome();
	
	//-------------------------------------------------------------------------
	// Genome operations

	// Randomize all gene values.
	void Randomize();

	static Genome* SpawnChild(Genome* p1, Genome* p2, Simulation* theSimulation);
	
	//-------------------------------------------------------------------------
	// Gene access

	float GetGeneAsFloat(unsigned int index) const;
	float GetGeneAsFloat(unsigned int index, float minValue, float maxValue) const;
	int GetGeneAsInt(unsigned int index, int minValue, int maxValue) const;
	unsigned int GetGeneAsInt(unsigned int index, unsigned int minValue, unsigned int maxValue) const;
	
	//-------------------------------------------------------------------------
	// Neurogenetics

	// Create a brain from the encoding in the neurological genes.
	void GrowBrain(Brain* brain);


private:
	std::vector<unsigned char> m_genes; // A gene is 1 byte. 0 = minimum value, 255 = maximum
	Simulation* m_simulation;
};


#endif // _GENOME_H_