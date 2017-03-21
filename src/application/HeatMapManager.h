#ifndef _HEAT_MAP_MANAGER_H_
#define _HEAT_MAP_MANAGER_H_

#include <simulation/Simulation.h>
#include <string>
#include "GraphManager.h"


typedef float (*GetAgentFloatValueCallback) (Agent* agent);


//-----------------------------------------------------------------------------
// HeatMapInfo
//-----------------------------------------------------------------------------
class HeatMapInfo
{
public:
	enum DataType
	{
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_BYTE,
		TYPE_UNSIGNED_BYTE,
		TYPE_SHORT,
		TYPE_UNSIGNED_SHORT,
		TYPE_UNSIGNED_INT,
		TYPE_INT,
		TYPE_LONG,
		TYPE_UNSIGNED_LONG,
	};

	// Constructors
	HeatMapInfo();
	HeatMapInfo(const std::string& title, GetAgentFloatValueCallback callback);

	// Getters
	inline const Color& GetColor() const { return m_color; }
	inline const std::string& GetTitle() const { return m_title; }
	inline const GraphRange& GetRange() const { return m_range; }
	float GetData(Agent* agent) const;

	// Setters
	HeatMapInfo& SetTitle(const std::string& title);
	HeatMapInfo& SetColor(const Color& color);
	HeatMapInfo& SetRange(const GraphRange& range);
	//HeatMapInfo& SetData(int dataOffset, DataType dataType);

private:
	GetAgentFloatValueCallback	m_callback;
	std::string		m_title;
	Color			m_color;
	GraphRange		m_range;
};


//-----------------------------------------------------------------------------
// HeatMapManager
//-----------------------------------------------------------------------------
class HeatMapManager
{
public:
	HeatMapManager();
	~HeatMapManager();

	void Initialize();
	void OnNewSimulation(Simulation* simulation);

	inline unsigned int GetNumHeatMap() const { return m_heatMaps.size(); }
	inline HeatMapInfo* GetHeatMap(unsigned int index) { return m_heatMaps[index]; }

	HeatMapInfo& AddHeatMap(const HeatMapInfo& heatMapInfo);

private:

	std::vector<HeatMapInfo*> m_heatMaps;
};


#endif // _HEAT_MAP_MANAGER_H_