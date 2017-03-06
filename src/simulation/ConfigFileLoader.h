#ifndef _CONFIG_FILE_LOADER_H_
#define _CONFIG_FILE_LOADER_H_

#include <simulation/SimulationConfig.h>
#include <string>


class ConfigFileLoader
{
public:
	ConfigFileLoader();

	bool LoadConfigFile(const std::string& fileName, SimulationConfig& outConfig);

private:
	void ParseLine(const std::string& line);

	SimulationConfig* m_outConfig;
	std::string m_fileName;
};


#endif // _CONFIG_FILE_LOADER_H_