#include "FileUtility.h"


bool FileUtility::OpenAndGetContents(const std::string& fileName, std::string& outContents)
{
	// Open the file.
	FILE* m_file = fopen(fileName.c_str(), "r");
	if (m_file == nullptr)
		return false;

	// Determine the file size.
	if (fseek(m_file, 0, SEEK_END) != 0)
		return false;
	long fileSize = ftell(m_file);
	if (fileSize < 0)
		return false;
	rewind(m_file);

	// Read the entire file into a character buffer.
	const size_t fileSizeInclNull = fileSize + 1;
	char* buffer = new char[fileSizeInclNull];
	size_t result = fread(buffer, 1, fileSizeInclNull, m_file);
	if (result == 0)
	{
		delete buffer;
		return false;
	}

	// Finalize the buffer as the output string.
	buffer[result] = '\0';
	outContents.assign(buffer, buffer + result);

	delete buffer;
	return true;
}

