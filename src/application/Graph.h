#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <graphics/Color.h>
#include <graphics/SpriteFont.h>
#include <graphics/Graphics.h>
#include <math/Rect2i.h>
#include <vector>
#include <string>


struct Bounds
{
	Vector2f mins;
	Vector2f maxs;

	Bounds() :
		mins(0, 0),
		maxs(0, 0)
	{}

	static Bounds Union(const Bounds& a, const Bounds& b);
};

class Graph
{
public:
	Graph(std::string name, const Color& color);

	const std::string&	GetName()	const { return m_name; }
	const Color&		GetColor()	const { return m_color; }
	const Bounds&		GetBounds()	const { return m_bounds; }

	int		GetDataCount()		const { return m_dataCount; }
	float	GetData(int index)	const { return *((float*) (m_data + m_dataOffset + (index * m_dataStride))); }
	
	void ConfigData(const void* data, int count, int stride, int offset)
	{
		m_data = (const char*) data;
		m_dataCount = count;
		m_dataStride = stride;
		m_dataOffset = offset;
	}

private:
	Color		m_color;
	std::string	m_name;
	Bounds		m_bounds;
	const char*	m_data;
	int			m_dataCount; // number of data points
	int			m_dataOffset; // offset in bytes
	int			m_dataStride; // stride in bytes
};


class GraphPanel
{
public:
	GraphPanel();
	~GraphPanel();
	
	Graph* GetGraph() { return m_graphs[0]; }
	//void GetGraphRange(const GraphInfo& graph, float& rangeMin,
		//float& rangeMax, float& domainMax, float& domainMin);

	inline void SetTitle(const std::string& title) { m_title = title; }
	inline void SetFont(SpriteFont* font) { m_font = font; }
	void SetViewport(const Viewport& viewport);
	void SetViewport(const Rect2i& viewport);
	void SetXBounds(float minX, float maxX);
	void SetYBounds(float minY, float maxY);
	void SetDynamicRange(bool dynamicRange, float dynamicRangePadding = 0.1f);

	Graph* AddGraph(const std::string& name, const Color& color);
	Graph* GetGraph(const std::string& name);
	
	void Draw(Graphics& g);

private:
	Vector2f GetPointOnGraph(const Vector2f& point);
	void DrawGraph(Graphics* g, Graph* graph);

	SpriteFont*	m_font;

	std::string	m_title;
	std::vector<Graph*> m_graphs;
	Viewport	m_viewport;
	Viewport	m_graphViewport;
	Bounds		m_minBounds;
	Bounds		m_bounds;
	bool		m_dynamicRange;
	float		m_dynamicRangePadding;
};


#endif // _GRAPH_H_