#include "ConfigFileLoader.h"
#include <math/MathLib.h>
#include <utilities/Logging.h>
#include <utilities/StringUtility.h>
#include <fstream>
#include <sstream>
#include <vector>


//-----------------------------------------------------------------------------
// ConfigParam
//-----------------------------------------------------------------------------

unsigned int ConfigParam::GetDataTypeSize(DataType dataType)
{
	if (dataType == TYPE_FLOAT)			return sizeof(float);
	if (dataType == TYPE_INT)			return sizeof(int);
	if (dataType == TYPE_UNSIGNED_INT)	return sizeof(unsigned int);
	if (dataType == TYPE_BOOL)			return sizeof(bool);
	return 0;
}


//-----------------------------------------------------------------------------
// ConfigUnit
//-----------------------------------------------------------------------------

ConfigUnit::ConfigUnit(const std::string& names,
		ConfigParam::Units units, float conversionFactor) :
	units(units),
	conversionFactor(conversionFactor)
{
	this->names = StringUtility::Tokenize(names, ";, \t", false);
}

bool ConfigUnit::ConvertValue(void* value, unsigned int dataType)
{
	if (dataType == ConfigParam::TYPE_FLOAT)
	{
		*((float*) value) = *((float*) value) * conversionFactor;
	}
	else if (dataType == ConfigParam::TYPE_INT)
	{
		*((int*) value) = (int) ((float) *((int*) value) *
			conversionFactor + 0.5f);
	}
	else if (dataType == ConfigParam::TYPE_INT)
	{
		*((unsigned int*) value) = (unsigned int)
			((float) *((unsigned int*) value) * conversionFactor + 0.5f);
	}
	else
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
// Macro functions
//-----------------------------------------------------------------------------

// Single parameters

#define ADD_PARAM(name, type, units) \
	m_params[#name] = new ConfigParam(#name, \
		offsetof(SimulationConfig, name), type, units)

#define ADD_INT_PARAM(name, units) \
	ADD_PARAM(name, ConfigParam::TYPE_INT, units)

#define ADD_FLOAT_PARAM(name, units) \
	ADD_PARAM(name, ConfigParam::TYPE_FLOAT, units)

#define ADD_BOOL_PARAM(name, units) \
	ADD_PARAM(name, ConfigParam::TYPE_BOOL, units)

#define ADD_COLOR_PARAM(name) \
	m_params[#name ".red"] = new ConfigParam(#name ".red",\
		offsetof(SimulationConfig, name[0]), \
		ConfigParam::TYPE_FLOAT, ConfigParam::UNITS_NONE); \
	m_params[#name ".green"] = new ConfigParam(#name ".green", \
		offsetof(SimulationConfig, name[1]), \
		ConfigParam::TYPE_FLOAT, ConfigParam::UNITS_NONE); \
	m_params[#name ".blue"] = new ConfigParam(#name ".blue", \
		offsetof(SimulationConfig, name[2]), \
		ConfigParam::TYPE_FLOAT, ConfigParam::UNITS_NONE)

// Species parameters

#define LINK_SPECIES_PARM(name) \
	m_params["herbivore." #name]->otherSpeciesParam = m_params["carnivore." #name]; \
	m_params["carnivore." #name]->otherSpeciesParam = m_params["herbivore." #name]

#define ADD_SPECIES_INT_PARAM(name, units) \
	ADD_INT_PARAM(carnivore.name, units); \
	ADD_INT_PARAM(herbivore.name, units); \
	LINK_SPECIES_PARM(name)

#define ADD_SPECIES_FLOAT_PARAM(name, units) \
	ADD_FLOAT_PARAM(carnivore.name, units); \
	ADD_FLOAT_PARAM(herbivore.name, units); \
	LINK_SPECIES_PARM(name)

#define ADD_SPECIES_BOOL_PARAM(name, units) \
	ADD_BOOL_PARAM(carnivore.name, units); \
	ADD_BOOL_PARAM(herbivore.name, units); \
	LINK_SPECIES_PARM(name)

#define ADD_SPECIES_COLOR_PARAM(name) \
	ADD_COLOR_PARAM(carnivore.name); \
	ADD_COLOR_PARAM(herbivore.name); \
	LINK_SPECIES_PARM(name.red); \
	LINK_SPECIES_PARM(name.green); \
	LINK_SPECIES_PARM(name.blue)



//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

ConfigFileLoader::ConfigFileLoader() :
	m_outConfig(nullptr)
{
	//-------------------------------------------------------------------------
	// Simulation parameters

	// World
	ADD_FLOAT_PARAM	(world.radius,						ConfigParam::UNITS_DISTANCE);
	ADD_INT_PARAM	(world.seed,						ConfigParam::UNITS_NONE);
	ADD_INT_PARAM	(world.matingSeasonDuration,		ConfigParam::UNITS_TIME);
	ADD_INT_PARAM	(world.offSeasonDuration,			ConfigParam::UNITS_TIME);
	
	// Plants
	ADD_FLOAT_PARAM	(plant.radius,						ConfigParam::UNITS_DISTANCE);
	ADD_INT_PARAM	(plant.numPlants,					ConfigParam::UNITS_NONE);
	ADD_COLOR_PARAM	(plant.color);
	ADD_INT_PARAM	(plant.numOffshootsPerPlant,		ConfigParam::UNITS_NONE);
	ADD_FLOAT_PARAM	(plant.maxEnergy,					ConfigParam::UNITS_NONE);
	ADD_FLOAT_PARAM	(plant.offshootSpawnRadius,			ConfigParam::UNITS_DISTANCE);
	ADD_FLOAT_PARAM	(plant.growthRate,					ConfigParam::UNITS_NONE);		// energy / time
	ADD_FLOAT_PARAM	(plant.eatEnergyDepletionRate,		ConfigParam::UNITS_NONE);		// energy / time

	//-------------------------------------------------------------------------
	// Species parameters

	// Population size
	ADD_SPECIES_INT_PARAM	(population.initialAgents,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(population.minAgents,				ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(population.maxAgents,				ConfigParam::UNITS_NONE);
	
	// Fittest list
	ADD_SPECIES_INT_PARAM	(fittestList.numFittestAgents,		ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(fittestList.mateTournamentSize,	ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(fittestList.mateChance,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(fittestList.randomChance,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(fittestList.eliteChance,			ConfigParam::UNITS_NONE);
	
	// General agent config
	ADD_SPECIES_INT_PARAM	(agent.matingDelay,					ConfigParam::UNITS_TIME);
	ADD_SPECIES_FLOAT_PARAM	(agent.minMatingDistance,			ConfigParam::UNITS_DISTANCE);
	ADD_SPECIES_FLOAT_PARAM	(agent.radiusAtMinStrength,			ConfigParam::UNITS_DISTANCE);
	ADD_SPECIES_FLOAT_PARAM	(agent.radiusAtMaxStrength,			ConfigParam::UNITS_DISTANCE);
	ADD_SPECIES_FLOAT_PARAM	(agent.maxEnergyAtMinStrength,		ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(agent.maxEnergyAtMaxStrength,		ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(agent.maxMoveSpeedAtMinStrength,	ConfigParam::UNITS_DISTANCE); // distance / time
	ADD_SPECIES_FLOAT_PARAM	(agent.maxMoveSpeedAtMaxStrength,	ConfigParam::UNITS_DISTANCE); // distance / time
	ADD_SPECIES_FLOAT_PARAM	(agent.maxTurnSpeedAtMinStrength,	ConfigParam::UNITS_ANGLE); // angle / time
	ADD_SPECIES_FLOAT_PARAM	(agent.maxTurnSpeedAtMaxStrength,	ConfigParam::UNITS_ANGLE); // angle / time
	ADD_SPECIES_BOOL_PARAM	(agent.collisions,					ConfigParam::UNITS_NONE);
		
	// Energy costs
	ADD_SPECIES_FLOAT_PARAM	(energy.energyCostExist,			ConfigParam::UNITS_NONE); // energy / time
	ADD_SPECIES_FLOAT_PARAM	(energy.energyCostMove,				ConfigParam::UNITS_NONE); // energy / distance
	ADD_SPECIES_FLOAT_PARAM	(energy.energyCostTurn,				ConfigParam::UNITS_NONE); // energy / angle
	ADD_SPECIES_FLOAT_PARAM	(energy.energyCostNeuron,			ConfigParam::UNITS_NONE); // energy / time
	ADD_SPECIES_FLOAT_PARAM	(energy.energyCostSynapse,			ConfigParam::UNITS_NONE); // energy / time
		
	// Reproduction genes
	ADD_SPECIES_FLOAT_PARAM	(genes.minMutationRate,				ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(genes.maxMutationRate,				ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.minCrossoverPoints,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.maxCrossoverPoints,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.minChildren,					ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.maxChildren,					ConfigParam::UNITS_NONE);

	// Physiological genes
	ADD_SPECIES_FLOAT_PARAM	(genes.minStrength,					ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(genes.maxStrength,					ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.minLifeSpan,					ConfigParam::UNITS_TIME);
	ADD_SPECIES_INT_PARAM	(genes.maxLifeSpan,					ConfigParam::UNITS_TIME);
	ADD_SPECIES_COLOR_PARAM	(genes.minBodyColor);
	ADD_SPECIES_COLOR_PARAM	(genes.maxBodyColor);
	ADD_SPECIES_FLOAT_PARAM	(genes.minFieldOfView,				ConfigParam::UNITS_ANGLE);
	ADD_SPECIES_FLOAT_PARAM	(genes.maxFieldOfView,				ConfigParam::UNITS_ANGLE);
	ADD_SPECIES_FLOAT_PARAM	(genes.minAngleBetweenEyes,			ConfigParam::UNITS_ANGLE);
	ADD_SPECIES_FLOAT_PARAM	(genes.maxAngleBetweenEyes,			ConfigParam::UNITS_ANGLE);
	ADD_SPECIES_FLOAT_PARAM	(genes.minSightDistance,			ConfigParam::UNITS_DISTANCE);
	ADD_SPECIES_FLOAT_PARAM	(genes.maxSightDistance,			ConfigParam::UNITS_DISTANCE);
	ADD_SPECIES_INT_PARAM	(genes.minSightResolution,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_INT_PARAM	(genes.maxSightResolution,			ConfigParam::UNITS_NONE);

	// Neurological genes
	ADD_SPECIES_INT_PARAM	(genes.maxInternalNeurons,			ConfigParam::UNITS_NONE);

	// Brain config
	ADD_SPECIES_INT_PARAM	(brain.numPrebirthCycles,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.maxBias,						ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.sigmoidSlope,				ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.maxWeight,					ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.initMaxWeight,				ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.weightLearningRate,			ConfigParam::UNITS_NONE);
	ADD_SPECIES_FLOAT_PARAM	(brain.weightDecayRate,				ConfigParam::UNITS_NONE);
	
	//-------------------------------------------------------------------------
	// Units

	m_units.push_back(new ConfigUnit("radians,radian,rads,rad,r", ConfigParam::UNITS_ANGLE, Math::RAD_TO_DEG));
	m_units.push_back(new ConfigUnit("degrees,degree,degs,deg,d", ConfigParam::UNITS_ANGLE, 1.0f));
	m_units.push_back(new ConfigUnit("seconds,second,secs,sec,s", ConfigParam::UNITS_TIME, 60.0f));
	m_units.push_back(new ConfigUnit("ticks,tick,frames,frame", ConfigParam::UNITS_TIME, 1.0f));
}

ConfigFileLoader::~ConfigFileLoader()
{
	// Delete config params.
	for (auto it = m_params.begin(); it != m_params.end(); ++it)
		delete it->second;

	// Delete config units.
	for (unsigned int i = 0; i < m_units.size(); ++i)
		delete m_units[i];
}


//-----------------------------------------------------------------------------
// File loading
//-----------------------------------------------------------------------------

// Load a simulation configuration from file into a config struct.
// On success, returns true. On Failure, returns false.
bool ConfigFileLoader::LoadConfigFile(const std::string& fileName,
	SimulationConfig& outConfig, std::string& errorMessage)
{
	m_fileName = fileName;
	m_outConfig = &outConfig;

	// Open the file.
	std::ifstream file(fileName);
	if (!file)
	{
		errorMessage = "Unable to open file '" + fileName + "'";
		return false;
	}

	errorMessage = "There were errors in the Config File:\n\n";
	
	// Initialize loading state
	for (auto it = m_params.begin(); it != m_params.end(); ++it)
	{
		it->second->isSet = false;
	}

	// Read the file line by line.
	std::string line;
	bool success = true;
	int lineNumber = 1;
	std::string errorText;
	std::stringstream errorStream;

	while (std::getline(file, line))
	{
		if (!ParseLine(line, errorText))
		{
			success = false;
			errorStream.str("");
			errorStream << "At line " << lineNumber <<
				": Error: " << errorText << "\n";
			errorMessage += errorStream.str();
			errorText = "";
		}
		lineNumber++;
	}

	file.close();
	return success;
}


//-----------------------------------------------------------------------------
// File parsing
//-----------------------------------------------------------------------------

// Parse a single line in a config file.
// On success, returns true. On Failure, returns false and sets outError.
bool ConfigFileLoader::ParseLine(const std::string& lineFromFile, std::string& outError)
{
	// Notes on the Config File Format:
	// inline comments with '#'
	// all white space is ignored
	// key = value 
	// semicolons are optional

	std::string line = lineFromFile;

	// Remove comments.
	int commentStart = line.find_first_of('#');
	if (commentStart != std::string::npos)
		line = line.substr(0, commentStart);

	// Trim whitespace.
	line = StringUtility::Trim(line);

	// Ignore empty lines.
	if (line.length() == 0)
		return true;

	// Find the position of the equals sign.
	int equalsPos = line.find_first_of('=');
	if (equalsPos == std::string::npos)
	{
		outError = "syntax error : lines must be in the format 'key = value'";
		return false;
	}

	// Split the line into key and value strings.
	std::string key = StringUtility::Trim(line.substr(0, equalsPos));
	std::string value = StringUtility::Trim(line.substr(equalsPos + 1));
	std::string label = "";

	// Tokenize the value, delimeted by whitespace.
	// The first token is the value number.
	// The second token is the label (units like 'ticks', 'radians', etc.).
	std::vector<std::string> tokens = StringUtility::Tokenize(value, " \t", false);
	if (tokens.empty())
	{
		outError = "expected a value";
		return false;
	}

	value = tokens[0];
	if (tokens.size() > 1)
		label = tokens[1];

	// Now actually parse the parameter key and value.
	return ParseParam(key, value, label, outError);
}

// Parse a parameter given strings for the key, value, and units label.
// On success, returns true. On Failure, returns false and sets outError.
bool ConfigFileLoader::ParseParam(const std::string& key,
	const std::string& value, const std::string& label, std::string& outError)
{
	// Find the parameter by key string.
	ConfigParam* param = GetParamInfo(key);
	if (param == nullptr)
	{
		outError = "'" + key + "' : invalid config parameter";
		return false;
	}

	if (value.length() == 0)
	{
		outError = "expected a value";
		return false;
	}
	
	void* dataPtr = ((char*) m_outConfig + param->offset);

	// First, parse the value string.
	if (!ParseValueParam(value, param->type, param->units, dataPtr, outError))
		return false;

	// Second, parse the label string.
	
	// Find the unit by label string.
	ConfigUnit* unit = nullptr;
	if (label.length() > 0)
	{
		unit = GetUnitInfo(label);

		if (unit == nullptr)
		{
			outError = "'" + label + "' : invalid unit type";
			return false;
		}
		else if (unit->units != param->units)
		{
			if (param->units == ConfigParam::UNITS_DISTANCE)
				outError = "'" + label + "' : "  + unit->names[0] + " is the wrong units for distance";
			else if (param->units == ConfigParam::UNITS_TIME)
				outError = "'" + label + "' : "  + unit->names[0] + " is the wrong units for time";
			else if (param->units == ConfigParam::UNITS_NONE)
				outError = "'" + label + "' : "  + unit->names[0] + " cannot be used for a unitless value";
			else
				outError = "'" + label + "' : "  + unit->names[0] + " is the wrong units for '" + key + "'";
			return false;
		}
	}

	// Convert the data to the standard units.
	if (unit != nullptr)
	{
		if (unit->names[0] == "radians")
			int a = 0;
		if (!unit->ConvertValue(dataPtr, param->type))
		{
			outError = "value conversion error";
			return false;
		}
	}

	// If this parameter is a species parameter it hasn't been
	// set manually for the other species yet, then set this value
	// automatically for the other species as well.
	if (param->otherSpeciesParam != nullptr &&
		!param->otherSpeciesParam->isSet)
	{
		void* otherDataPtr = ((char*) m_outConfig +
			param->otherSpeciesParam->offset);
		memcpy(otherDataPtr, dataPtr, ConfigParam::GetDataTypeSize(param->type));
	}
	
	param->isSet = true;
	return true;
}

// Parse the value string of a single config parameter.
// On success, returns true. On Failure, returns false and sets outError.
bool ConfigFileLoader::ParseValueParam(const std::string& value,
	ConfigParam::DataType dataType, ConfigParam::Units unitsType,
	void* dataPtr, std::string& outError)
{
	char* endPtr;

	// Parse the value for the correct data type.
	if (dataType == ConfigParam::TYPE_FLOAT)
	{
		float floatValue = strtof(value.c_str(), &endPtr);
		if (endPtr != value.c_str() + value.length())
		{
			outError = "'" + value.substr(endPtr - value.c_str()) + "' : unexpected token";
			return false;
		}
		if (unitsType == ConfigParam::UNITS_ANGLE)
			floatValue = Math::ToRadians(floatValue); // angles by default are in degrees
		*((float*) dataPtr) = floatValue;
	}
	else if (dataType == ConfigParam::TYPE_BOOL)
	{
		// Convert the string to lowercase first.
		std::string str = value;
		for (unsigned int i = 0; i < str.length(); ++i)
			str[i] = tolower(str[i]);
		bool isTrue  = (str == "true"  || str == "1" || str == "on"  || str == "yes");
		bool isFalse = (str == "false" || str == "0" || str == "off" || str == "no");
		if (!isTrue && !isFalse)
		{
			outError = "'" + value + "' : unexpected token";
			return false;
		}
		*((bool*) dataPtr) = isTrue;
	}
	else if (dataType == ConfigParam::TYPE_INT)
	{
		int intValue = (int) strtol(value.c_str(), &endPtr, 10);
		if (endPtr != value.c_str() + value.length())
		{
			outError = "'" + value.substr(endPtr - value.c_str()) + "' : unexpected token";
			return false;
		}
		*((int*) dataPtr) = intValue;
	}
	else if (dataType == ConfigParam::TYPE_UNSIGNED_INT)
	{
		unsigned int uintValue = (unsigned int) strtoul(value.c_str(), &endPtr, 10);
		if (endPtr != value.c_str() + value.length())
		{
			outError = "'" + value.substr(endPtr - value.c_str()) + "' : unexpected token";
			return false;
		}
		*((unsigned int*) dataPtr) = uintValue;
	}
	else
	{
		outError = "unsupported parameter data type";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Parameter and units queries
//-----------------------------------------------------------------------------

ConfigUnit* ConfigFileLoader::GetUnitInfo(const std::string& unitsName)
{
	// Find the unit that has the given name.
	for (unsigned int i = 0; i < m_units.size(); ++i)
	{
		for (unsigned int j = 0; j < m_units[i]->names.size(); ++j)
		{
			if (m_units[i]->names[j] == unitsName)
				return m_units[i];
		}
	}
	return nullptr;
}

ConfigParam* ConfigFileLoader::GetParamInfo(const std::string& paramName)
{
	// Find the parameter that has the given name.
	auto it = m_params.find(paramName);
	if (it == m_params.end())
		return nullptr;
	return it->second;
}


