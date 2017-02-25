#include "GraphManager.h"


//-----------------------------------------------------------------------------
// GraphRange
//-----------------------------------------------------------------------------

GraphRange::GraphRange() :
	minType(FIXED),
	minValue(0.0f),
	maxType(DYNAMIC),
	maxValue(1.0f),
	dynamicPaddingPercent(0.1f)
{
}

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
	char* rawData = ((char*) &stats->species[m_species]) + m_dataOffset;
	//char* rawData = ((char*) stats) + m_dataOffset;

	switch (m_dataType)
	{
	case TYPE_FLOAT:
		return *((float*) rawData);
	case TYPE_DOUBLE:
		return (float) *((double*) rawData);
	case TYPE_BYTE:
		return (float) *rawData;
	case TYPE_UNSIGNED_BYTE:
		return (float) *((unsigned char*) rawData);
	case TYPE_SHORT:
		return (float) *((short*) rawData);
	case TYPE_UNSIGNED_SHORT:
		return (float) *((unsigned short*) rawData);
	case TYPE_INT:
		return (float) *((int*) rawData);
	case TYPE_UNSIGNED_INT:
		return (float) *((unsigned int*) rawData);
	case TYPE_LONG:
		return (float) *((long*) rawData);
	case TYPE_UNSIGNED_LONG:
		return (float) *((unsigned long*) rawData);
	default:
		return 0.0f;
	}
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
	AddGraph(GraphInfo()).SetTitle(title).SetData(offsetof(SpeciesStats, statName), GraphInfo::TYPE_FLOAT).SetColor(color)


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

	GraphRange zeroToOne;
	zeroToOne.SetFixedRange(0, 1);
	
	GraphRange rangePop;
	rangePop.SetFixedMin(0.0f);
	rangePop.SetDynamicMax((float) config.herbivore.population.maxAgents);
	zeroAndUp.SetDynamicRangePadding(0.1f);

	Color blue(0, 106, 255);
	Color orange(255, 128, 0);

	// General
	CREATE_GRAPH_FLOAT("Population Size", populationSize, Color::CYAN).SetRange(rangePop);
	CREATE_GRAPH_FLOAT("Total Energy", totalEnergy, Color::YELLOW).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Avg Energy", avgEnergy, Color::YELLOW).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Avg Energy Usage", avgEnergyUsage, Color::YELLOW).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Avg Fitness", avgFitness, Color::GREEN).SetRange(zeroAndUp);
	CREATE_GRAPH_FLOAT("Avg Move Amount", avgMoveAmount, Color::GREEN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Turn Amount", avgTurnAmount, Color::YELLOW).SetRange(zeroToOne);

	// Average gene values
	CREATE_GRAPH_FLOAT("Avg Color Red", avgGeneValue[GenePosition::COLOR_RED], Color::RED).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Color Green", avgGeneValue[GenePosition::COLOR_GREEN], Color::GREEN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Color Blue", avgGeneValue[GenePosition::COLOR_BLUE], blue).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Lifespan", avgGeneValue[GenePosition::LIFE_SPAN], Color::MAGENTA).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Strength", avgGeneValue[GenePosition::STRENGTH], Color::RED).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Child Count", avgGeneValue[GenePosition::CHILD_COUNT], Color::GREEN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Mutation Rate", avgGeneValue[GenePosition::MUTATION_RATE], Color::MAGENTA).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Crossover Points", avgGeneValue[GenePosition::CROSSOVER_POINTS], Color::YELLOW).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Field of View", avgGeneValue[GenePosition::FIELD_OF_VIEW], Color::GREEN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Angle Between Eyes", avgGeneValue[GenePosition::ANGLE_BETWEEN_EYES], orange).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Sight Distance", avgGeneValue[GenePosition::VIEW_DISTANCE], Color::CYAN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Resolution Red", avgGeneValue[GenePosition::RESOLUTION_RED], Color::RED).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Resolution Green", avgGeneValue[GenePosition::RESOLUTION_GREEN], Color::GREEN).SetRange(zeroToOne);
	CREATE_GRAPH_FLOAT("Avg Resolution Blue", avgGeneValue[GenePosition::RESOLUTION_BLUE], blue).SetRange(zeroToOne);
}

GraphInfo& GraphManager::AddGraph(const GraphInfo& graphInfo)
{
	GraphInfo* newGraph = new GraphInfo(graphInfo);
	m_graphs.push_back(newGraph);
	return *newGraph;
}
