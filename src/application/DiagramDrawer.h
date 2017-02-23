#ifndef _DIAGRAM_DRAWER_H_
#define _DIAGRAM_DRAWER_H_

#include <graphics/Graphics.h>
#include <application/GraphManager.h>

class SimulationManager;
class Simulation;


class DiagramDrawer
{
public:
	DiagramDrawer(SimulationManager* simulationManager);
	
	void DrawBrainMatrix(Graphics& g, Agent* agent, const Rect2f& bounds);
	void DrawGraph(Graphics& g, const GraphInfo& graph, const Rect2f& bounds);


private:
	SimulationManager* m_simulationManager;
};


#endif // _DIAGRAM_DRAWER_H_