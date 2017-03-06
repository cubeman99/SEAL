#ifndef _TIME_H_
#define _TIME_H_


//-----------------------------------------------------------------------------
// Time - Static class for portable time utility functions
//-----------------------------------------------------------------------------
class Time
{
public:
	
	// Get the current time in seconds since epoch.
	static double GetTime();

private:
	// this is meant to be a static class.
	Time() {}
};


#endif // _TIME_H_