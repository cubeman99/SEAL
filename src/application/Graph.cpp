#include "Graph.h"
#include <math/MathLib.h>
#include <graphics/OpenGLIncludes.h>


Bounds Bounds::Union(const Bounds& a, const Bounds& b)
{
	Bounds result;
	result.mins.x = Math::Min(a.mins.x, b.mins.x);
	result.mins.y = Math::Min(a.mins.y, b.mins.y);
	result.maxs.x = Math::Max(a.maxs.x, b.maxs.x);
	result.maxs.y = Math::Max(a.maxs.y, b.maxs.y);
	return result;
}



Graph::Graph(std::string name, const Color& color) :
	m_name(name),
	m_color(color),
	m_data(nullptr),
	m_dataCount(0),
	m_dataStride(sizeof(float)),
	m_dataOffset(0)
{
}




GraphPanel::GraphPanel() :
	m_dynamicRange(true),
	m_dynamicRangePadding(0.1f),
	m_font(nullptr)
{
}

GraphPanel::~GraphPanel()
{
	for (unsigned int i = 0; i < m_graphs.size(); i++)
		delete m_graphs[i];
}


void GraphPanel::SetXBounds(float minX, float maxX)
{
	m_bounds.mins.x = minX;
	m_bounds.maxs.x = maxX;
	m_minBounds.mins.x = minX;
	m_minBounds.maxs.x = maxX;
}

void GraphPanel::SetYBounds(float minY, float maxY)
{
	m_bounds.mins.y = minY;
	m_bounds.maxs.y = maxY;
	m_minBounds.mins.y = minY;
	m_minBounds.maxs.y = maxY;
}

void GraphPanel::SetDynamicRange(bool dynamicRange, float dynamicRangePadding)
{
	m_dynamicRange = dynamicRange;
	m_dynamicRangePadding = dynamicRangePadding;
}

void GraphPanel::SetViewport(const Viewport& viewport)
{
	m_viewport = viewport;
}


Graph* GraphPanel::AddGraph(const std::string& name, const Color& color)
{
	Graph* graph = new Graph(name, color);
	m_graphs.push_back(graph);
	return graph;
}

Graph* GraphPanel::GetGraph(const std::string& name)
{
	for (unsigned int i = 0; i < m_graphs.size(); i++)
	{
		if (m_graphs[i]->GetName() == name)
			return m_graphs[i];
	}
	return NULL;
}

void GraphPanel::Draw(Graphics& g2)
{
	Graphics* g = &g2;

	Color m_colorBackground	= Color::BLACK;
	Color m_colorGraphArea	= Color::BLACK;
	Color m_colorOutline	= Color::WHITE;
	Color colorZeroY		= Color::WHITE;
	Color colorZeroX		= Color::WHITE;
	Color colorGraphMargin	= Color::LIGHT_GRAY;
	Color colorLabels		= Color::WHITE;
	
	//-----------------------------------------------------------------------------
	// Calculate view bounds
	
	Bounds dynamicBounds = m_minBounds;

	// Calculate the domain and range of the data.
	for (unsigned int i = 0; i < m_graphs.size(); i++)
		dynamicBounds = Bounds::Union(dynamicBounds, m_graphs[i]->GetBounds());
	
	// Fit the view to the data's range.
	if (m_dynamicRange)
	{
		// Add in some padding so the graph doesn't touch the edges.
		float range = dynamicBounds.maxs.y - dynamicBounds.mins.y;
		dynamicBounds.mins.y -= range * m_dynamicRangePadding * 0.5f;
		dynamicBounds.maxs.y += range * m_dynamicRangePadding * 0.5f;
		m_bounds.mins.y = dynamicBounds.mins.y;
		m_bounds.maxs.y = dynamicBounds.maxs.y;
	}
		
	// Fit the view to the data's domain.
	m_bounds.mins.x = dynamicBounds.mins.x;
	m_bounds.maxs.x = dynamicBounds.maxs.x;

	//-----------------------------------------------------------------------------

	char labelMin[32];
	char labelMax[32];
	sprintf_s(labelMin, "%.2f", m_bounds.mins.y);
	sprintf_s(labelMax, "%.2f", m_bounds.maxs.y);
	int labelMinLength = strnlen_s(labelMin, 32);
	int labelMaxLength = strnlen_s(labelMax, 32);
	int labelTextLength = Math::Max(labelMinLength, labelMaxLength);
	
	int padding		= 6;
	int labelWidth	= 8 * labelTextLength;
	int labelHeight	= 12;
	int titleWidth	= m_title.length() * 8;
	int titleHeight	= 12;

	m_graphViewport.x		= labelWidth + (padding * 2);
	m_graphViewport.y		= padding + titleHeight;
	m_graphViewport.width	= m_viewport.width - (padding * 3) - labelWidth;
	m_graphViewport.height	= m_viewport.height - (padding * 2) - titleHeight;

	//-----------------------------------------------------------------------------
	// GRAPH BACKGROUND.

	glDisable(GL_LINE_SMOOTH);
	g->SetViewport(m_viewport, true);
	g->SetProjection(Matrix4f::CreateOrthographic(
		(float) m_viewport.x,
		(float) m_viewport.x + m_viewport.width,
		(float) m_viewport.y + m_viewport.height,
		(float) m_viewport.y,
		-1.0f, 1.0f));
	
	g->ResetTransform();
	
	// Draw the background and outline.
	g->FillRect(m_viewport, m_colorBackground);
	g->DrawRect(m_viewport, m_colorOutline);

	g->Translate(Vector2f((float) m_viewport.x, (float) m_viewport.y));
	
	// Draw the graph area's background.
	g->FillRect(m_graphViewport, m_colorGraphArea);

	// Draw the graph area's left margin.
	g->DrawLine((float) m_graphViewport.x,
				(float) m_graphViewport.y,
				(float) m_graphViewport.x,
				(float) m_graphViewport.y + m_graphViewport.height,
				colorGraphMargin);
	
	// Draw tick marks at the top and bottom of the graph area's left margin.
	int tickMarkWidth = 6;
	g->DrawLine((float) m_graphViewport.x - (tickMarkWidth / 2),
				(float) m_graphViewport.y,
				(float) m_graphViewport.x + (tickMarkWidth / 2),
				(float) m_graphViewport.y,
				colorGraphMargin);
	g->DrawLine((float) m_graphViewport.x - (tickMarkWidth / 2),
				(float) m_graphViewport.y + m_graphViewport.height,
				(float) m_graphViewport.x + (tickMarkWidth / 2),
				(float) m_graphViewport.y + m_graphViewport.height,
				colorGraphMargin);

	// Draw the y-axis labels.
	g->DrawString(m_font, labelMax,
		(float) padding + labelWidth - (labelMaxLength * 8),
		(float) m_graphViewport.y - (labelHeight / 2),
		colorLabels);
	g->DrawString(m_font, labelMin,
		(float) padding + labelWidth - (labelMinLength * 8),
		(float) m_graphViewport.y + m_graphViewport.height - (labelHeight / 2),
		colorLabels);

	g->DrawString(m_font, m_title.c_str(),
		(float) (m_viewport.width / 2) - (titleWidth / 2),
		(float) padding,
		colorLabels);
	
	//-----------------------------------------------------------------------------
	// GRAPH PLOTS.

	// Draw the line for y = 0 and x = 0.
	Vector2f origin = GetPointOnGraph(Vector2f(0.0f, 0.0f));
	if (0.0f > m_bounds.mins.y && 0.0f < m_bounds.maxs.y)
		g->DrawLine((float) m_graphViewport.x, origin.y, (float) m_graphViewport.x + m_graphViewport.width, origin.y, colorZeroY);
	if (0.0f > m_bounds.mins.x && 0.0f < m_bounds.maxs.x)
		g->DrawLine(origin.x, (float) m_graphViewport.y, origin.x, (float) m_graphViewport.y + m_graphViewport.height, colorZeroX);

	// Draw the individual graphs.
	for (unsigned int i = 0; i < m_graphs.size(); i++)
	{
		DrawGraph(g, m_graphs[i]);
	}
}

Vector2f GraphPanel::GetPointOnGraph(const Vector2f& point)
{
	return Vector2f(
		m_graphViewport.x + (point.x - m_bounds.mins.x) / (m_bounds.maxs.x - m_bounds.mins.x) * m_graphViewport.width,
		m_graphViewport.y + m_graphViewport.height - ((point.y - m_bounds.mins.y) / (m_bounds.maxs.y - m_bounds.mins.y) * m_graphViewport.height));
}

void GraphPanel::GetGraphRange(const GraphInfo& graph, float& rangeMin,
	float& rangeMax, float& domainMin, float& domainMax)
{
	for (unsigned int i = 0; i < m_graphs.size(); ++i)
	{
		for (int j = 0; j < m_graphs[i]->GetDataCount(); ++j)
		{
			float x = (float) i;
			float y = m_graphs[i]->GetData(j);

			if (y < rangeMin)
				rangeMin = y;
			if (y > rangeMax)
				rangeMax = y;
			if ((float) x < domainMin)
				domainMin = (float) x;
			if ((float) x > domainMax)
				domainMax = (float) x;
		}
	}
}

void GraphPanel::DrawGraph(Graphics* g, Graph* graph)
{
	glBegin(GL_LINE_STRIP);
	glColor4ubv(graph->GetColor().data());

	for (int i = 0; i < graph->GetDataCount(); i++)
	{
		float x = (float) i;
		float y = graph->GetData(i);

		if (x >= m_bounds.mins.x && x <= m_bounds.maxs.x)
		{
			Vector2f point = GetPointOnGraph(Vector2f(x, y));
			glVertex2fv(point.data());
		}
	}
	
	glEnd();
}

