#ifndef _CONFIG_FILE_LOADER_H_
#define _CONFIG_FILE_LOADER_H_

#include <simulation/SimulationConfig.h>
#include <string>
#include <map>


struct ConfigParam
{
public:
	enum DataType
	{
		//TYPE_STRING,
		TYPE_FLOAT,
		//TYPE_DOUBLE,
		//TYPE_BYTE,
		//TYPE_UNSIGNED_BYTE,
		//TYPE_SHORT,
		//TYPE_UNSIGNED_SHORT,
		TYPE_UNSIGNED_INT,
		TYPE_INT,
		//TYPE_LONG,
		//TYPE_UNSIGNED_LONG,
	};

public:

	ConfigParam()
	{
	}

	ConfigParam(const std::string& name, unsigned int offset, unsigned int type) :
		name(name),
		offset(offset),
		type(type)
	{
	}

	std::string		name;
	unsigned int	offset;
	unsigned int	type;
	//std::string	description;
	// min/max
};


class ConfigFileLoader
{
public:
	ConfigFileLoader();

	bool LoadConfigFile(const std::string& fileName, SimulationConfig& outConfig);

private:
	void ParseLine(const std::string& line);

	SimulationConfig* m_outConfig;
	std::string m_fileName;

	// Map of all possible parameters for a config file
	std::map<std::string, ConfigParam> m_params;
};


#endif // _CONFIG_FILE_LOADER_H_