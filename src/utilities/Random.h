#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <iostream>
#include <ctime>


//-----------------------------------------------------------------------------
// Random - Static random number generator which uses rand().
//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
// RandomNumberGenerator - A linear congruential generator implementation of
//                         a random number generator.
//-----------------------------------------------------------------------------
class RandomNumberGenerator
{
public:

	// The largest number that NextInt() can return.
	static const int RANDOM_MAX = 32767;
	
public:

	//-------------------------------------------------------------------------
	// Constructors

	// Initiaze the generator with a seed of 1.
	inline RandomNumberGenerator() :
		m_seed(1)
	{}

	// Initiaze the generator with the given seed.
	inline RandomNumberGenerator(unsigned long seed) :
		m_seed(seed)
	{}


	//-------------------------------------------------------------------------
	// Random number generation

	// Return an random integer between 0 and RANDOM_MAX (inclusive).
	inline int NextInt()
    {
        m_seed = (m_seed * 1103515245) + 12345;
        return (int) ((m_seed / 65536) % 32768);
	}

	// Return a random integer between minValue (inclusive)
	// and maxValue (exclusive).
	inline int NextInt(int minValue, int maxValue)
	{
		return (minValue + (NextInt() % (maxValue - minValue)));
	}
	
	// Return a random boolean value.
	inline bool NextBool()
	{
		return ((NextInt() % 2) == 0);
	}

	// Return a random float between 0 and 1 (inclusive)
	inline float NextFloat()
	{
		return static_cast<float> (NextInt()) /
			static_cast<float> (RANDOM_MAX);
	}

	// Return a random float between -1 and +1 (inclusive).
	inline float NextFloatClamped()
	{
		return (NextFloat() - NextFloat());
	}

	// Return a random float between minValue and maxValue (inclusive).
	inline float NextFloat(float minValue, float maxValue)
	{
		return (minValue + (NextFloat() * (maxValue - minValue)));
	}


	//-------------------------------------------------------------------------
	// Seeding 

	// Set the current seed to the given number.
	inline void SetSeed(unsigned long seed)
    {
        m_seed = seed;
    }
	
	// Set the current seed to the current time (obtained from time(0)).
	// The exact seed will be the number of seconds elapsed since midnight,
	// January 1, 1970. Therefore, calling this function multiple times within
	// the span of 1 second can be expected to yield some matching seeds.
	inline void SeedTime()
	{
		m_seed = static_cast<unsigned long> (time(0));
	}

	// Get the current seed. Note that the seed changes every time a
	// random value is generated.
    inline unsigned long GetSeed() const
    {
        return m_seed;
    }

private:
	unsigned long m_seed; // The current seed used to generate a random number.
};


typedef RandomNumberGenerator RNG;


#endif // _RANDOM_H_