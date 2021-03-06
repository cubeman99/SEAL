#ifndef _FILE_UTILITY_H_
#define _FILE_UTILITY_H_

#include <string>


//-----------------------------------------------------------------------------
// FileUtility - Static class for file utility functions
//-----------------------------------------------------------------------------
class FileUtility
{
public:
	
	// Open and read an entire text file into a string.
	static bool OpenAndGetContents(const std::string& path, std::string& outContents);

private:
	// this is meant to be a static class.
	FileUtility() {}
};


#endif // _FILE_UTILITY_H_