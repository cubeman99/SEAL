#include "DiagramDrawer.h"
#include <simulation/Simulation.h>
#include <application/SimulationManager.h>
#include <sstream>


DiagramDrawer::DiagramDrawer(SimulationManager* simulationManager) :
	m_simulationManager(simulationManager)
{

}



void DiagramDrawer::DrawBrainMatrix(Graphics& g, Agent* agent, const Rect2f& bounds)
{
	const SimulationConfig& config =
		m_simulationManager->GetSimulation()->GetConfig();
	Brain* brain = agent->GetBrain();

	unsigned int numNeurons = brain->GetNumNeurons();
	unsigned int numInputNeurons = brain->GetNumInputNeurons();
	unsigned int numOutputNeurons = brain->GetNumOutputNeurons();
	unsigned int numOutIntNeurons = numNeurons - numInputNeurons;
	
	// Fit the brain to the width of the window.
	Vector2f cellSize = Vector2f((bounds.GetWidth() * 0.7f) / numNeurons);
	cellSize.y = cellSize.x;

	Vector2f center(bounds.GetWidth() * 0.5f, 100);
	Vector2f matrixSize(cellSize.x * numNeurons, cellSize.y * numOutIntNeurons);
	Vector2f matrixTopLeft(center.x - (matrixSize.x * 0.5f), 100);
	Vector2f matrixBottomRight = matrixTopLeft + cellSize *
		Vector2f((float) numNeurons, (float) numOutIntNeurons);
	Color outlineColor = Color::WHITE;

	// Draw connection matrix.
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		Neuron neuron = brain->GetNeuron(i);

		for (unsigned int k = neuron.synapsesBegin; k < neuron.synapsesEnd; ++k)
		{
			Synapse synapse = brain->GetSynapse(k);

			Vector2f cellPos;
			cellPos.x = matrixTopLeft.x + (cellSize.x * synapse.neuronFrom);
			cellPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons -
				(synapse.neuronTo - numInputNeurons) - 1));
			
			Vector3f weightColor(0, 0, 0);
			if (synapse.weight > 0.0f)
				weightColor[1] = 1.0f;
			if (synapse.weight < 0.0f)
				weightColor[0] = 1.0f;
			weightColor *= Math::Abs(synapse.weight) / config.brain.maxWeight;
			if (synapse.learningRate == 0.0f)
				weightColor = Vector3f(0.3f, 0.3f, 0.3f);

			// Fill synapse box with color.
			g.FillRect(cellPos, cellSize, weightColor);
			
			// Draw synapse box outline.
			if (synapse.learningRate != 0.0f)
			{
				weightColor.Set(0, 0, 0);
				if (synapse.learningRate > 0.0f)
					weightColor[1] = 1.0f;
				if (synapse.learningRate < 0.0f)
					weightColor[0] = 1.0f;
				weightColor *= (Math::Abs(synapse.weight) / config.brain.maxWeight) + 0.2f;

				g.DrawRect(cellPos, cellSize, weightColor);
			}
		}
	}

	// Draw neuron activations.
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		Neuron neuron = brain->GetNeuron(i);
		float activation = brain->GetNeuronActivation(i);

		Vector2f cellPos;
		cellPos.x = matrixTopLeft.x + (cellSize.x * i);
		cellPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1));
			
		Vector3f weightColor(1, 1, 1);

		// Colorize input neurons.
		if (i == 0)
			weightColor.Set(1, 1, 0);
		else if (i == 1)
			weightColor.Set(1, 0, 1);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 2))
			weightColor.Set(1, 0, 0);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 4))
			weightColor.Set(0, 1, 0);
		else if ((int) i < 2 + (config.genes.maxSightResolution * 6))
			weightColor.Set(0, 0, 1);

		// Fill box color.
		g.FillRect(cellPos, cellSize, weightColor * activation);
		
		// Draw box outline.
		g.DrawRect(cellPos, cellSize, weightColor * (activation + 0.2f));
	}

	// Draw neuron group seperation lines.
	glBegin(GL_LINES);
	glColor3f(1,1,1);
	glVertex2f(matrixTopLeft.x + cellSize.x * numInputNeurons, matrixTopLeft.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * numInputNeurons, matrixBottomRight.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * (numInputNeurons + numOutputNeurons), matrixTopLeft.y);
	glVertex2f(matrixTopLeft.x + cellSize.x * (numInputNeurons + numOutputNeurons), matrixBottomRight.y);
	glVertex2f(matrixTopLeft.x, matrixBottomRight.y - cellSize.y * numOutputNeurons);
	glVertex2f(matrixBottomRight.x, matrixBottomRight.y - cellSize.y * numOutputNeurons);
	glEnd();
		
	// Draw activations strip outline.
	Vector2f activationsPos;
	activationsPos.x = matrixTopLeft.x;
	activationsPos.y = matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1));
	g.DrawRect(
		matrixTopLeft.x - 1,
		matrixTopLeft.y + (cellSize.y * (numOutIntNeurons + 1)) - 1,
		cellSize.x * numNeurons + 2,
		cellSize.y + 2,
		outlineColor);

	// Draw connection matrix outlines.
	Vector2f one(1, 1);
	Vector2f two(2, 2);
	g.DrawRect(matrixTopLeft - one, matrixBottomRight -
		matrixTopLeft + two, outlineColor);
}


void DiagramDrawer::DrawGraph(Graphics& g, const GraphInfo& graph, const Rect2f& rect)
{
	Font* m_font = nullptr;
	Simulation* simulation = m_simulationManager->GetSimulation();
	SimulationStats* stats = simulation->m_generationStats.data();
	unsigned int numStats = simulation->m_generationStats.size();

	Color colorBackground	= Color::BLACK;
	Color colorGraphArea	= Color::BLACK;
	Color colorOutline		= Color::WHITE;
	Color colorZeroY		= Color::WHITE;
	Color colorZeroX		= Color::WHITE;
	Color colorGraphMargin	= Color::LIGHT_GRAY;
	Color colorLabels		= Color::WHITE;

	//-------------------------------------------------------------------------
	// Calculate view bounds.

	const GraphRange& range = graph.GetRange();

	Vector2f mins;
	Vector2f maxs;
	for (unsigned int i = 0; i < numStats; ++i)
	{
		float x = (float) i;
		float y = graph.GetData(stats + i);
		if (i == 0 || x < mins.x) mins.x = x;
		if (i == 0 || y < mins.y) mins.y = y;
		if (i == 0 || x > maxs.x) maxs.x = x;
		if (i == 0 || y > maxs.y) maxs.y = y;
	}

	float rangeSpan = maxs.y - mins.y;

	//-------------------------------------------------------------------------
	// Adjust view bounds range for fixed or dynamic clamped ranges.

	if (range.minType == GraphRange::FIXED)
	{
		mins.y = range.minValue;
	}
	else
	{
		mins.y -= range.dynamicPaddingPercent * rangeSpan;
		if (range.minType == GraphRange::DYNAMIC_CLAMPED && mins.y > range.minValue)
			mins.y = range.minValue;
	}
	if (range.maxType == GraphRange::FIXED)
	{
		maxs.y = range.maxValue;
	}
	else
	{
		maxs.y += range.dynamicPaddingPercent * rangeSpan;
		if (range.maxType == GraphRange::DYNAMIC_CLAMPED && maxs.y < range.maxValue)
			maxs.y = range.maxValue;
	}

	//-------------------------------------------------------------------------
	// Determine range label strings and graph area rectangle.

	std::stringstream str;
	str << mins.y;
	std::string labelMin = str.str();

	str.str("");
	str << maxs.y;
	std::string labelMax = str.str();

	int labelMinLength = labelMin.length();
	int labelMaxLength = labelMax.length();
	int labelTextLength = Math::Max(labelMinLength, labelMaxLength);
	
	int padding		= 6;
	int labelWidth	= 8 * labelTextLength;
	int labelHeight	= 12;
	int titleWidth	= graph.GetTitle().length() * 8;
	int titleHeight	= 12;
	
	Rect2f graphRect = rect;
	graphRect.position.x	+= labelWidth + (padding * 2);
	graphRect.position.y	+= padding + titleHeight;
	graphRect.size.x		= rect.size.x - (padding * 3) - labelWidth;
	graphRect.size.y		= rect.size.y - (padding * 2) - titleHeight;

	//-------------------------------------------------------------------------
	
	g.SetViewportToCanvas();
	g.SetCanvasProjection();

	// Fill background
	g.FillRect(rect, colorBackground);

	// Fill graph area background
	g.FillRect(graphRect, colorGraphArea);
	
	// Draw the graph area's left margin.
	g.DrawLine((float) graphRect.position.x,
				(float) graphRect.position.y,
				(float) graphRect.position.x,
				(float) graphRect.position.y + graphRect.size.y,
				colorGraphMargin);
	
	// Draw tick marks at the top and bottom of the graph area's left margin.
	int tickMarkWidth = 6;
	g.DrawLine((float) graphRect.position.x - (tickMarkWidth / 2),
				(float) graphRect.position.y,
				(float) graphRect.position.x + (tickMarkWidth / 2),
				(float) graphRect.position.y,
				colorGraphMargin);
	g.DrawLine((float) graphRect.position.x - (tickMarkWidth / 2),
				(float) graphRect.position.y + graphRect.size.y,
				(float) graphRect.position.x + (tickMarkWidth / 2),
				(float) graphRect.position.y + graphRect.size.y,
				colorGraphMargin);
	
	//-------------------------------------------------------------------------
	// Draw labels.

	// Draw the y-axis labels.
	g.DrawString(m_font, labelMax,
		graphRect.position.x - padding,
		graphRect.position.y,
		colorLabels, TextAlign::MIDDLE_RIGHT);
	g.DrawString(m_font, labelMin,
		graphRect.position.x - padding,
		graphRect.position.y + graphRect.size.y,
		colorLabels, TextAlign::MIDDLE_RIGHT);

	// Draw title
	g.DrawString(m_font, graph.GetTitle(),
		rect.position.x + (float) (rect.size.x / 2),
		rect.position.y + (float) padding,
		colorLabels, TextAlign::TOP_CENTER);

	//-------------------------------------------------------------------------
	
	// Setup projection for graph view bounds.
	Matrix4f viewBoundsProjection;
	viewBoundsProjection.InitOrthographic(mins.x, maxs.x, mins.y, maxs.y, -1, 1);
	g.SetViewport(Rect2i((int) graphRect.position.x,
		(int) graphRect.position.y, (int) graphRect.size.x, (int) graphRect.size.y), true);
	g.SetProjection(viewBoundsProjection);
	
	// Draw the graph line.
	glBegin(GL_LINE_STRIP);
	glColor4ubv(graph.GetColor().data());
	for (unsigned int i = 0; i < numStats; ++i)
	{
		float x = (float) i;
		float y = graph.GetData(stats + i);
		if (i == 0 && x < mins.x) mins.x = x;
		if (i == 0 && y < mins.y) mins.y = y;
		if (i == 0 && x > maxs.x) maxs.x = x;
		if (i == 0 && y > maxs.y) maxs.y = y;
		glVertex2f(x, y);
	}
	glEnd();

	g.SetViewportToCanvas();
	g.SetCanvasProjection();
	
	//-------------------------------------------------------------------------

	// Draw graph outline
	//g.DrawRect(graphRect, colorOutline);

	// Draw graph outline
	g.DrawRect(rect, colorOutline);
}

