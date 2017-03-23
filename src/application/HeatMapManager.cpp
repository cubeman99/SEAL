#include "HeatMapManager.h"



HeatMapInfo::HeatMapInfo() :
	m_title("unknown"),
	m_callback(nullptr),
	m_color(Color::YELLOW)
{
}

HeatMapInfo::HeatMapInfo(const std::string& title, GetAgentFloatValueCallback callback) :
	m_title(title),
	m_callback(callback),
	m_color(Color::YELLOW)
{

}

float HeatMapInfo::GetData(Agent* agent) const
{
	if (m_callback)
		return m_callback(agent);
	return 0.0f;
}

HeatMapInfo& HeatMapInfo::SetTitle(const std::string& title)
{
	m_title = title;
	return *this;
}

HeatMapInfo& HeatMapInfo::SetColor(const Color& color)
{
	m_color = color;
	return *this;
}

HeatMapInfo& HeatMapInfo::SetRange(const GraphRange& range)
{
	m_range = range;
	return *this;
}


//-----------------------------------------------------------------------------
// HeatMapManager
//-----------------------------------------------------------------------------

#define CREATE_HEAT_MAP(title, color, agentFloatGetterFunc) \
	AddHeatMap(HeatMapInfo(title, [](Agent* agent) { \
		return (float) agent->agentFloatGetterFunc(); \
	})).SetColor(color)

#define CREATE_HEAT_MAP_CUSTOM(title, color, agentFloatGetterCode) \
	AddHeatMap(HeatMapInfo(title, [](Agent* agent)\
		agentFloatGetterCode \
	)).SetColor(color)

#define CREATE_GENE_HEAT_MAP(title, color, geneIndex) \
	AddHeatMap(HeatMapInfo(title, [](Agent* agent) { \
		return agent->GetGenome()->GetGeneAsFloat(geneIndex); \
	})).SetColor(color).SetRange(GraphRange().SetFixedRange(0.0f, 1.0f))

HeatMapManager::HeatMapManager()
{
}

HeatMapManager::~HeatMapManager()
{
	for (unsigned int i = 0; i < m_heatMaps.size(); ++i)
		delete m_heatMaps[i];
	m_heatMaps.clear();
}

void HeatMapManager::Initialize()
{
}

void HeatMapManager::OnNewSimulation(Simulation* simulation)
{
	SimulationConfig config = simulation->GetConfig();
	
	for (unsigned int i = 0; i < m_heatMaps.size(); ++i)
		delete m_heatMaps[i];
	m_heatMaps.clear();
	
	GraphRange zeroAndUp;
	zeroAndUp.SetFixedMin(0.0f);
	zeroAndUp.SetDynamicMax();
	zeroAndUp.SetDynamicRangePadding(0.1f);

	GraphRange zeroToOne;
	zeroToOne.SetFixedRange(0, 1);
	
	Color blue(0, 106, 255);
	Color orange(255, 128, 0);

	// Agent state heat maps:

	CREATE_HEAT_MAP("Age",				Color::MAGENTA,	GetAge).SetRange(zeroAndUp);
	CREATE_HEAT_MAP("Energy",			Color::YELLOW,	GetEnergy).SetRange(zeroAndUp);
	CREATE_HEAT_MAP("Energy Usage",		Color::YELLOW,	GetEnergyUsage).SetRange(zeroAndUp);
	CREATE_HEAT_MAP("Health Energy",	Color::RED,		GetHealthEnergy).SetRange(zeroAndUp);
	CREATE_HEAT_MAP("Fitness",			Color::GREEN,	GetFitness).SetRange(zeroAndUp);
	CREATE_HEAT_MAP("Move Amount",		Color::GREEN,	GetMoveAmount).SetRange(zeroToOne);

	CREATE_HEAT_MAP_CUSTOM("Turn Amount", Color::CYAN, {
		return Math::Abs(agent->GetTurnAmount());
	}).SetRange(zeroToOne);

	// Gene heat maps:

	CREATE_GENE_HEAT_MAP("Gene - Color Red",			Color::RED,			COLOR_RED);
	CREATE_GENE_HEAT_MAP("Gene - Color Green",			Color::GREEN,		COLOR_GREEN);
	CREATE_GENE_HEAT_MAP("Gene - Color Blue",			Color::BLUE,		COLOR_BLUE);
	CREATE_GENE_HEAT_MAP("Gene - Life Span",			Color::MAGENTA,		LIFE_SPAN);
	CREATE_GENE_HEAT_MAP("Gene - Strength",				Color::RED,			STRENGTH);
	CREATE_GENE_HEAT_MAP("Gene - Mutation Rate",		Color::MAGENTA,		MUTATION_RATE);
	CREATE_GENE_HEAT_MAP("Gene - Crossover Points",		Color::YELLOW,		CROSSOVER_POINTS);
	CREATE_GENE_HEAT_MAP("Gene - Child Count",			Color::GREEN,		CHILD_COUNT);
	CREATE_GENE_HEAT_MAP("Gene - Field of View",		Color::GREEN,		FIELD_OF_VIEW);
	CREATE_GENE_HEAT_MAP("Gene - Angle Between Eyes",	orange,				ANGLE_BETWEEN_EYES);
	CREATE_GENE_HEAT_MAP("Gene - Sight Distance",		Color::CYAN,		VIEW_DISTANCE);
	CREATE_GENE_HEAT_MAP("Gene - Resolution Red",		Color::RED,			RESOLUTION_RED);
	CREATE_GENE_HEAT_MAP("Gene - Resolution Green",		Color::GREEN,		RESOLUTION_GREEN);
	CREATE_GENE_HEAT_MAP("Gene - Resolution Blue",		Color::BLUE,		RESOLUTION_BLUE);
}

HeatMapInfo& HeatMapManager::AddHeatMap(const HeatMapInfo& heatMapInfo)
{
	HeatMapInfo* newHeatMap = new HeatMapInfo(heatMapInfo);
	m_heatMaps.push_back(newHeatMap);
	return *newHeatMap;
}
