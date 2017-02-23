#ifndef _GRAPH_MANAGER_H_
#define _GRAPH_MANAGER_H_

#include "Graph.h"
#include <simulation/Simulation.h>
#include <string>


struct GraphRange
{
	enum
	{
		FIXED = 0,
		DYNAMIC,
		DYNAMIC_CLAMPED,
	};

	float	minValue;
	int		minType;
	float	maxValue;
	int		maxType;
	float	dynamicPaddingPercent;

	
	GraphRange& SetFixedMin(float rangeMin);
	GraphRange& SetFixedMax(float rangeMax);
	GraphRange& SetFixedRange(float rangeMin, float rangeMax);
	GraphRange& SetDynamicMin();
	GraphRange& SetDynamicMax();
	GraphRange& SetDynamicRange();
	GraphRange& SetDynamicMin(float largestRangeMin);
	GraphRange& SetDynamicMax(float smallestRangeMax);
	GraphRange& SetDynamicRange(float largestRangeMin, float smallestRangeMax);
	GraphRange& SetDynamicRangePadding(float dynamicPaddingPercent);


	GraphRange() :
		minType(FIXED),
		minValue(0.0f),
		maxType(DYNAMIC),
		maxValue(1.0f),
		dynamicPaddingPercent(0.1f)
	{
	}
};


class GraphInfo
{
public:
	enum DataType
	{
		TYPE_FLOAT,
		TYPE_DOUBLE,
		//TYPE_BYTE,
		//TYPE_UNSIGNED_BYTE,
		//TYPE_SHORT,
		//TYPE_UNSIGNED_SHORT,
		TYPE_UNSIGNED_INT,
		TYPE_INT,
		TYPE_LONG,
		TYPE_UNSIGNED_LONG,
	};

	// Constructors
	GraphInfo();
	GraphInfo(const std::string& title, int dataOffset);

	// Getters
	inline const Color& GetColor() const { return m_color; }
	inline const std::string& GetTitle() const { return m_title; }
	inline const GraphRange& GetRange() const { return m_range; }
	inline DataType GetDataType() const { return m_dataType; }
	inline int GetDataOffset() const { return m_dataOffset; }
	float GetData(SimulationStats* stats) const;

	// Setters
	GraphInfo& SetTitle(const std::string& title);
	GraphInfo& SetColor(const Color& color);
	GraphInfo& SetRange(const GraphRange& range);
	GraphInfo& SetData(int dataOffset, DataType dataType);


private:
	std::string		m_title;
	Color			m_color;
	GraphRange		m_range;
	int				m_dataOffset;
	DataType		m_dataType;
};


class GraphManager
{
public:
	GraphManager();
	~GraphManager();

	void Initialize();
	void OnNewSimulation(Simulation* simulation);

	inline unsigned int GetNumGraphs() const { return m_graphs.size(); }
	inline GraphInfo* GetGraph(unsigned int index) { return m_graphs[index]; }

	GraphInfo& AddGraph(const GraphInfo& graphInfo);

private:

	std::vector<GraphInfo*> m_graphs;
};


#endif // _GRAPH_MANAGER_H_