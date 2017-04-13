#include "Timing.h"
#include <time.h>


// Code by: thebennybox 
// Modified by: David Jordan
// https://github.com/BennyQBD/3DEngineCpp/blob/master/src/core/timing.cpp


// Define which operating system we are on.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
	#define OS_WINDOWS
#elif defined(__linux__)
	#define OS_LINUX
#else
	#define OS_OTHER
#endif


#if defined(OS_WINDOWS)
	#include <Windows.h>
	#include <iostream>
	static double g_freq;
	static bool g_timerInitialized = false;
	
#elif defined(OS_LINUX)
	#include <sys/time.h>
	static const long NANOSECONDS_PER_SECOND = 1000000000L;
	
#endif


double Time::GetTime()
{
	#if defined(OS_WINDOWS)

		// Initialize the timer if it hasn't been aleady.
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
		
		return (double(li.QuadPart) / g_freq);

	#elif defined(OS_LINUX)

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return (double) (((long) ts.tv_sec * NANOSECONDS_PER_SECOND) +
			ts.tv_nsec) / ((double) (NANOSECONDS_PER_SECOND));

		return 0.0; // Error: unsupported operating system.	
	#endif
}

