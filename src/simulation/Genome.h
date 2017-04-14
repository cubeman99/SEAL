#ifndef _GENOME_H_
#define _GENOME_H_

#include <vector>
#include <simulation/SimulationConfig.h>
#include <utilities/Random.h>

class Simulation;
class Brain;


//-----------------------------------------------------------------------------
// GenePosition - byte offsets for the different genes in a genome
//-----------------------------------------------------------------------------
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
	CROSSOVER_POINTS,
	CHILD_COUNT,

	// Physiological
	LIFE_SPAN,
	STRENGTH,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,

	PHYSIOLOGICAL_GENES_COUNT,

	// First index of nueron genes, also count of non-nueron genes
	NUERON_GENES_BEGIN = PHYSIOLOGICAL_GENES_COUNT,
};


//-----------------------------------------------------------------------------
// Genome - A collection of single-byte genes, that can be crossed-over and
//          mutated.
//-----------------------------------------------------------------------------
class Genome
{
public:
	//-------------------------------------------------------------------------
	// Constructor & destructor

	Genome(const Genome& copy);
	Genome(const SpeciesConfig& config);
	~Genome();
	
	//-------------------------------------------------------------------------
	// Genome operations

	// Randomize all gene values.
	void Randomize(RNG& random);

	// Calculate genome size based on agent species configuration.
	static int DetermineGenomeSize(const SpeciesConfig& config);

	// Create a child genome from two parent genomes,
	// through crossover and mutation.
	static Genome* SpawnChild(Genome* p1, Genome* p2,
		const SpeciesConfig& config, RNG& random);
	
	//-------------------------------------------------------------------------
	// Gene access

	const unsigned char* GetData() const { return m_genes.data(); }
	unsigned char* GetData() { return m_genes.data(); }
	unsigned int GetSize() const { return m_genes.size(); }

	float GetGeneAsFloat(unsigned int index) const;
	float GetGeneAsFloat(unsigned int index, float minValue, float maxValue) const;
	int GetGeneAsInt(unsigned int index, int minValue, int maxValue) const;
	unsigned int GetGeneAsInt(unsigned int index, unsigned int minValue, unsigned int maxValue) const;
	
	//-------------------------------------------------------------------------
	// Neurogenetics

	// Create a brain from the encoding in the neurological genes.
	void GrowBrain(Brain* brain, RNG& random, const SpeciesConfig& speciesConfig);


private:
	std::vector<unsigned char> m_genes; // A gene is 1 byte. 0 = minimum value, 255 = maximum
};


#endif // _GENOME_H_