#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <iostream>
#include <ctime>


// Static random number generator (using rand())
class Random
{
public:
	inline static void SeedTime()
	{
		srand((unsigned int) time(0));
	}

	inline static void Seed(unsigned int seed)
	{
		srand(seed);
	}

	inline static bool NextBool()
	{
		return ((rand() % 2) == 0);
	}

	inline static float NextFloat()
	{
		return static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
	}

	inline static float NextFloat(float minValue, float maxValue)
	{
		return (minValue + (NextFloat() * (maxValue - minValue)));
	}

	inline static float NextFloatClamped()
	{
		return (NextFloat() - NextFloat());
	}

	inline static int NextInt()
	{
		return rand();
	}

	// Inclusive to Exclusive
	inline static int NextInt(int min, int max)
	{
		return (min + (rand() % (max - min)));
	}
};


// Linear Congruential Generator
class RandomNumberGenerator
{
public:
	static const int RANDOM_MAX = 32767;
	
public:
	RandomNumberGenerator()
		: m_seed(1)
	{}

	RandomNumberGenerator(unsigned long seed)
		: m_seed(seed)
	{}

	//-----------------------------------------------------------------------------

	inline int NextInt()
    {
        m_seed = (m_seed * 1103515245) + 12345;
        return (unsigned int) (m_seed / 65536) % 32768;
    }
	
	inline bool NextBool()
	{
		return ((NextInt() % 2) == 0);
	}

	inline float NextFloat()
	{
		return static_cast<float> (NextInt()) / static_cast<float> (RANDOM_MAX);
	}

	inline float NextFloat(float minValue, float maxValue)
	{
		return (minValue + (NextFloat() * (maxValue - minValue)));
	}

	inline float NextFloatClamped()
	{
		return (NextFloat() - NextFloat());
	}

	//-----------------------------------------------------------------------------

    void SetSeed(unsigned long seed)
    {
        m_seed = seed;
    }
	
	inline void SeedTime()
	{
		m_seed = (unsigned long) time(0);
	}

    unsigned long GetSeed() const
    {
        return m_seed;
    }

private:
	unsigned long m_seed;
};

typedef RandomNumberGenerator RNG;


#endif // _RANDOM_H_