#ifndef _STRING_UTILITY_H_
#define _STRING_UTILITY_H_

#include <string>
#include <vector>


//-----------------------------------------------------------------------------
// StringUtility - Static class for string utility functions
//-----------------------------------------------------------------------------
class StringUtility
{
public:

	// Trim the whitespace (spaces and tabs) on both ends of a string.
	static std::string Trim(const std::string& str);

	// Separate a string into an array of tokens,
	// separated by the given delimeter characters.
	static std::vector<std::string> Tokenize(const std::string& str,
		const std::string& delimeter, bool includeEmptyTokens);
	

private:
	// this is meant to be a static class.
	StringUtility() {}
};


#endif // _STRING_UTILITY_H_