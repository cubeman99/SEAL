#include "ConfigFileLoader.h"
#include <fstream>
#include <sstream>


ConfigFileLoader::ConfigFileLoader() :
	m_outConfig(nullptr)
{
}


bool ConfigFileLoader::LoadConfigFile(const std::string& fileName, SimulationConfig& outConfig)
{
	m_fileName = fileName;
	m_outConfig = &outConfig;

	// Open the file.
	std::ifstream file(fileName);
	if (!file)
	{
		return false;
	}

	// Read the file line by line.
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream lineStream(line);

		ParseLine(line);
	}


	file.close();
	return true;
}


void ConfigFileLoader::ParseLine(const std::string& lineFromFile)
{
	// Notes on the Config File Format:
	// inline comments with '#'
	// all white space is ignored
	// key = value 
	// semicolons are optional

	int index;
	std::string line = lineFromFile;

	// Remove comments.
	index = line.find_first_of('#');
	if (index != std::string::npos)
		line = line.substr(0, index);

	// Trim white space.

	// Find the '=' character.

	// Split the line by key and value.

	// Update the config based on the key and value.
}

