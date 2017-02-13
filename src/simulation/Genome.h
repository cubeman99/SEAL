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

	// Physiological
	CHILD_COUNT,
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
	Genome(Simulation* theSimulation, bool randomized);
	~Genome();

	static Genome* SpawnChild(Genome* p1, Genome* p2);

	static float GeneLerp(float gene, float minValue, float maxValue);
	static int GeneLerp(float gene, int minValue, int maxValue);

	float GetGeneAsFloat(GenePosition index); // 0 - 1
	
	int GetGeneAsInt(unsigned int index, int minValue, int maxValue) const;
	float GetGeneAsFloat(unsigned int index, float minValue, float maxValue) const;
	unsigned int GetGeneAsInt(unsigned int index, unsigned int minValue, unsigned int maxValue) const;

	void GrowBrain(Brain* brain);

private:
	std::vector<unsigned char> m_genes; // A gene is 1 byte. 0 = 0%, 255 = 100%
	Simulation* m_simulation;
};


#endif // _GENOME_H_