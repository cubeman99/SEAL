#include "GraphManager.h"


//-----------------------------------------------------------------------------
// GraphRange
//-----------------------------------------------------------------------------

GraphRange& GraphRange::SetFixedMin(float rangeMin)
{
	minValue = rangeMin;
	minType = FIXED;
	return *this;
}

GraphRange& GraphRange::SetFixedMax(float rangeMax)
{
	maxValue = rangeMax;
	maxType = FIXED;
	return *this;
}

GraphRange& GraphRange::SetFixedRange(float rangeMin, float rangeMax)
{
	minValue = rangeMin;
	maxValue = rangeMax;
	minType = FIXED;
	maxType = FIXED;
	return *this;
}

GraphRange& GraphRange::SetDynamicMin()
{
	minType = DYNAMIC;
	return *this;
}

GraphRange& GraphRange::SetDynamicMax()
{
	maxType = DYNAMIC;
	return *this;
}

GraphRange& GraphRange::SetDynamicRange()
{
	minType = DYNAMIC;
	maxType = DYNAMIC;
	return *this;
}

GraphRange& GraphRange::SetDynamicMin(float largestRangeMin)
{
	minType = DYNAMIC_CLAMPED;
	minValue = largestRangeMin;
	return *this;
}

GraphRange& GraphRange::SetDynamicMax(float smallestRangeMax)
{
	maxType = DYNAMIC_CLAMPED;
	maxValue = smallestRangeMax;
	return *this;
}

GraphRange& GraphRange::SetDynamicRange(float largestRangeMin, float smallestRangeMax)
{
	minType = DYNAMIC_CLAMPED;
	minValue = largestRangeMin;
	maxType = DYNAMIC_CLAMPED;
	maxValue = smallestRangeMax;
	return *this;
}

GraphRange& GraphRange::SetDynamicRangePadding(float dynamicPaddingPercent)
{
	this->dynamicPaddingPercent = dynamicPaddingPercent;
	return *this;
}



//-----------------------------------------------------------------------------
// GraphInfo
//-----------------------------------------------------------------------------

GraphInfo::GraphInfo() :
	m_title(""),
	m_dataType(GraphInfo::TYPE_FLOAT),
	m_dataOffset(0),
	m_color(Color::WHITE)
{
}

GraphInfo::GraphInfo(const std::string& title, int dataOffset) :
	m_title(title),
	m_dataType(GraphInfo::TYPE_FLOAT),
	m_dataOffset(dataOffset),
	m_color(Color::WHITE)
{
}
	
float GraphInfo::GetData(SimulationStats* stats) const
{
	char* rawData = ((char*) stats) + m_dataOffset;
	if (m_dataType == TYPE_FLOAT)
		return *((float*) rawData);
	if (m_dataType == TYPE_DOUBLE)
		return (float) *((double*) rawData);
	if (m_dataType == TYPE_INT)
		return (float) *((int*) rawData);
	if (m_dataType == TYPE_UNSIGNED_INT)
		return (float) *((unsigned int*) rawData);
	if (m_dataType == TYPE_LONG)
		return (float) *((long*) rawData);
	if (m_dataType == TYPE_UNSIGNED_LONG)
		return (float) *((unsigned long*) rawData);
	return 0.0f;
}

GraphInfo& GraphInfo::SetTitle(const std::string& title)
{
	m_title = title;
	return *this;
}

GraphInfo& GraphInfo::SetColor(const Color& color)
{
	m_color = color;
	return *this;
}

GraphInfo& GraphInfo::SetRange(const GraphRange& range)
{
	m_range = range;
	return *this;
}

GraphInfo& GraphInfo::SetData(int dataOffset, DataType dataType)
{
	m_dataOffset = dataOffset;
	m_dataType = dataType;
	return *this;
}




//-----------------------------------------------------------------------------
// GraphManager
//-----------------------------------------------------------------------------

#define CREATE_GRAPH_FLOAT(title, statName, color) \
	AddGraph(GraphInfo()).SetTitle(title).SetData(offsetof(SimulationStats, statName), GraphInfo::TYPE_FLOAT).SetColor(color)


GraphManager::GraphManager()
{
}

GraphManager::~GraphManager()
{
	for (unsigned int i = 0; i < m_graphs.size(); ++i)
		delete m_graphs[i];
	m_graphs.clear();
}

void GraphManager::Initialize()
{

}

void GraphManager::OnNewSimulation(Simulation* simulation)
{
	SimulationConfig config = simulation->GetConfig();

	m_graphs.clear();

	GraphRange zeroAndUp;
	zeroAndUp.SetFixedMin(0.0f);
	zeroAndUp.SetDynamicMax();
	zeroAndUp.SetDynamicRangePadding(0.1f);
	
	GraphRange rangePop;
	rangePop.SetFixedMin(0.0f);
	//rangePop.SetFixedMax(config.agent.maxPreyAgents);
	rangePop.SetDynamicMax(config.agent.maxPreyAgents);

	CREATE_GRAPH_FLOAT("Population Size", populationSize, Color::CYAN).SetRange(rangePop);
	CREATE_GRAPH_FLOAT("Average Energy", avgEnergy, Color::YELLOW).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Total Energy", totalEnergy, Color::YELLOW).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Simulation Age", simulationAge, Color::MAGENTA).SetRange(zeroAndUp);
}

GraphInfo& GraphManager::AddGraph(const GraphInfo& graphInfo)
{
	GraphInfo* newGraph = new GraphInfo(graphInfo);
	m_graphs.push_back(newGraph);
	return *newGraph;
}
