#ifndef _DIAGRAM_DRAWER_H_
#define _DIAGRAM_DRAWER_H_

#include <graphics/Graphics.h>
#include <graphics/SpriteFont.h>
#include <application/GraphManager.h>

class SimulationManager;
class Simulation;


//-----------------------------------------------------------------------------
// DiagramDrawer - Draws diagrams like graphs and stuff.
//-----------------------------------------------------------------------------
class DiagramDrawer
{
public:
	DiagramDrawer(SimulationManager* simulationManager);
	
	void Initialize();

	void DrawBrainMatrix(Graphics& g, Agent* agent, const Rect2f& bounds);

	void DrawGraph(Graphics& g, const GraphInfo& graph, const Rect2f& bounds);
	void DrawGraphs(Graphics& g, const GraphInfo* graphs, unsigned int numGraphs, const Rect2f& bounds);

	inline Font* GetFont() const { return m_font; }


private:

	void CalcGraphRange(const GraphInfo& graph, float& rangeMin, float& rangeMax);

private:
	SimulationManager* m_simulationManager;
	Font* m_font;
};


#endif // _DIAGRAM_DRAWER_H_