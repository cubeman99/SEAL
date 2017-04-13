#ifndef _CONFIG_FILE_LOADER_H_
#define _CONFIG_FILE_LOADER_H_

#include <simulation/SimulationConfig.h>
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------
// ConfigParam - a single configurable parameter for a simulation
//-----------------------------------------------------------------------------
struct ConfigParam
{
public:
	// Supported data types.
	enum DataType
	{
		TYPE_FLOAT,
		TYPE_BOOL,
		TYPE_INT, TYPE_UNSIGNED_INT,
	};

	// Types of units.
	enum Units
	{
		UNITS_NONE,
		UNITS_DISTANCE,
		UNITS_ANGLE,
		UNITS_TIME,
	};

	// Get the size in bytes of a data type.
	static unsigned int GetDataTypeSize(DataType dataType);

public:
	std::string		name;
	unsigned int	offset;
	DataType		type;
	Units			units;
	bool			isSet;
	ConfigParam*	otherSpeciesParam; // link to this parameter for the opposite species.
	//std::string	description;
	// min/max

	ConfigParam(const std::string& name, unsigned int offset, DataType type, Units units) :
		name(name),
		offset(offset),
		type(type),
		units(units),
		isSet(false),
		otherSpeciesParam(nullptr)
	{
	}
};


//-----------------------------------------------------------------------------
// ConfigUnit - a valid units label that can be specified for a config
//              parameters value.
//-----------------------------------------------------------------------------
struct ConfigUnit
{
public:
	ConfigUnit(const std::string& names, ConfigParam::Units units, float conversionFactor);

	// Convert a value to the standard units.
	bool ConvertValue(void* value, unsigned int dataType);

public:
	std::vector<std::string> names;
	ConfigParam::Units units;
	float conversionFactor; // Factor to multiply values by to convert them to standard units.
};


//-----------------------------------------------------------------------------
// ConfigFileLoader - loads SimulationConfig from file.
//-----------------------------------------------------------------------------
class ConfigFileLoader
{
public:
	ConfigFileLoader();
	~ConfigFileLoader();

	//-------------------------------------------------------------------------
	// File loading
	
	// Load a simulation configuration from file into a config struct.
	// On success, returns true. On Failure, returns false and sets errorMessage.
	bool LoadConfigFile(const std::string& fileName,
		SimulationConfig& outConfig, std::string& errorMessage);


private:
	//-------------------------------------------------------------------------
	// File parsing

	// Parse a single line in a config file.
	// On success, returns true. On Failure, returns false and sets outError.
	bool ParseLine(const std::string& line, std::string& outError);

	// Parse a parameter given strings for the key, value, and units label.
	// On success, returns true. On Failure, returns false and sets outError.
	bool ParseParam(const std::string& key, const std::string& value,
		const std::string& label, std::string& outError);
	
	// Parse the value string of a single config parameter.
	// On success, returns true. On Failure, returns false and sets outError.
	bool ParseValueParam(const std::string& value, ConfigParam::DataType dataType,
		ConfigParam::Units unitsType, void* dataPtr, std::string& outError);
	
	//-------------------------------------------------------------------------
	// Parameter and units queries

	// Return the ConfigUnit with the given name (or null if it doesn exist).
	ConfigUnit* GetUnitInfo(const std::string& unitsName);

	// Return the ConfigParam with the given name (or null if it doesn exist).
	ConfigParam* GetParamInfo(const std::string& paramName);


private:
	SimulationConfig* m_outConfig;
	std::string m_fileName;

	// Map of all possible parameters for a config file
	std::map<std::string, ConfigParam*> m_params;

	// List of all possible units
	std::vector<ConfigUnit*> m_units;
};


#endif // _CONFIG_FILE_LOADER_H_