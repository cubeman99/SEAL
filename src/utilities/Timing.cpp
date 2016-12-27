#include "Timing.h"
#include <time.h>
#include <Windows.h>
#include <iostream>

static double g_freq;
static bool g_timerInitialized = false;

namespace Time
{
	double GetTime()
	{
		if (!g_timerInitialized)
		{
			LARGE_INTEGER li;
			if (!QueryPerformanceFrequency(&li))
				std::cerr << "QueryPerformanceFrequency failed in timer initialization"  << std::endl;
			
			g_freq = double(li.QuadPart);
			g_timerInitialized = true;
		}
	
		LARGE_INTEGER li;
		if (!QueryPerformanceCounter(&li))
			std::cerr << "QueryPerformanceCounter failed in get time!" << std::endl;
		
		return double(li.QuadPart) / g_freq;
	}
}
