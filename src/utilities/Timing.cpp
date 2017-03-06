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
#elif __cplusplus >= 201103L
	#define OS_OTHER_CPP11
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
	
#elif defined(OS_OTHER_CPP11)
	#include <chrono>
	static std::chrono::system_clock::time_point m_epoch =
		std::chrono::high_resolution_clock::now();
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

	#elif defined(OS_OTHER_CPP11)

		// TODO: apparently this only has 1 second precision.

		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now() -
			m_epoch).count() / 1000000000.0;

	#else
		return 0.0; // Error: unsupported operating system.	
	#endif
}

