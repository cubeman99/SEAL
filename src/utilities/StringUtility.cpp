#include "StringUtility.h"


// Trim the whitespace (spaces and tabs) on both ends of a string.
std::string StringUtility::Trim(const std::string& str)
{
	std::string result = str;

	size_t index = result.find_first_not_of(" \t");
	if (index != std::string::npos)
		result = result.substr(index);

	index = result.find_last_not_of(" \t");
	if (index != std::string::npos)
		result = result.substr(0, index + 1);

	return result;
}

// Separate a string into an array of tokens, delimited by the given delimeter characters.
std::vector<std::string> StringUtility::Tokenize(const std::string& str,
	const std::string& delimeter, bool includeEmptyTokens)
{
	std::vector<std::string> tokens;
	int tokenEnd;
	int tokenBegin = 0;
	int length = (int) str.length();

	while (tokenBegin < length)
	{
		// Find the next whitespace (or end of string).
		tokenEnd = str.find_first_of(delimeter, tokenBegin);
		if (tokenEnd == std::string::npos)
			tokenEnd = length;

		// Add the token to the list.
		if (tokenEnd > tokenBegin || includeEmptyTokens)
			tokens.push_back(str.substr(tokenBegin, tokenEnd - tokenBegin));

		// Move onto the next token.
		tokenBegin = tokenEnd + 1;
	}

	return tokens;
}

