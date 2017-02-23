#include "InfoPanel.h"
#include <sstream>
#include <math/Rect2f.h>
#include <math/MathLib.h>


InfoPanelItem::InfoPanelItem() :
	m_labelText("?"),
	m_labelColor(Color::WHITE),

	m_valueType(TYPE_FLOAT),
	m_valueColor(Color::WHITE),
	m_valueText("?"),
	m_valueSuffix(""),
	m_valueFloat(0.0f),
	m_valueInt(0),
	m_valuePrecision(2),

	m_visualType(VISUAL_NONE),
	m_visualColor(Color::YELLOW),
	m_visualBarMinValue(0.0f),
	m_visualBarMaxValue(1.0f),
	m_visualBarCenterValue(0.0f)
{
}

InfoPanelItem& InfoPanelItem::SetLabel(const std::string& text)
{
	m_labelText = text;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetLabel(const std::string& text, const std::string& valueSuffix)
{
	m_labelText = text;
	m_valueSuffix = valueSuffix;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetLabelColor(const Color& color)
{
	m_labelColor = color;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetValue(const std::string& text)
{
	m_valueType = 0;
	m_valueText = text;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetValue(float value)
{
	m_valueType = 1;
	m_valueFloat = value;
	m_valueInt = (int) value;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetValue(unsigned int value)
{
	m_valueType = 2;
	m_valueFloat = (float) value;
	m_valueInt = (int) value;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetValue(int value)
{
	m_valueType = 2;
	m_valueFloat = (float) value;
	m_valueInt = value;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetPrecision(int precision)
{
	m_valuePrecision = precision;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetValueColor(const Color& color)
{
	m_valueColor = color;
	return *this;
}

InfoPanelItem& InfoPanelItem::InitBar(const Color& color,
	int minValue, int maxValue, int centerValue)
{
	return InitBar(color, (float) minValue, (float) maxValue, (float) centerValue);
}

InfoPanelItem& InfoPanelItem::InitBar(const Color& color,
	unsigned int minValue, unsigned int maxValue, unsigned int centerValue)
{
	return InitBar(color, (float) minValue, (float) maxValue, (float) centerValue);
}

InfoPanelItem& InfoPanelItem::InitBar(const Color& color,
	float minValue, float maxValue, float centerValue)
{
	m_visualType = 1;
	m_visualColor = color;
	m_visualBarMinValue = minValue;
	m_visualBarMaxValue = maxValue;
	m_visualBarCenterValue = centerValue;
	return *this;
}

InfoPanelItem& InfoPanelItem::InitSolidColor(const Color& color)
{
	m_visualType = 2;
	m_visualColor = color;
	return *this;
}

InfoPanelItem& InfoPanelItem::SetVisualColor(const Color& color)
{
	m_visualColor = color;
	return *this;
}

void InfoPanelItem::UpdateValueText()
{
	if (m_valueType == TYPE_FLOAT)
	{
		std::stringstream ss;

		if (m_valuePrecision >= 0)
		{
			ss.setf(std::ios::fixed, std::ios::floatfield);
			ss.precision(m_valuePrecision);
		}

		ss << m_valueFloat << m_valueSuffix;
		m_valueText = ss.str();
	}
	else if (m_valueType == TYPE_INT)
	{
		std::stringstream ss;
		ss << m_valueInt << m_valueSuffix;
		m_valueText = ss.str();
	}
}


InfoPanel::InfoPanel()
{

}

Vector2f InfoPanel::GetMinimumSize() const
{
	float labelColWidth = 150;
	float valueColWidth = 80;
	float visualColWidth = 60;
	float colWidth = labelColWidth + valueColWidth + visualColWidth;
	float rowHeight = 12;
	float rowSeparation = 2;
	float titleHeight = 20;
	int numRows = (int) m_rows.size();

	Vector2f size;
	size.x = colWidth + (m_margin * 2);
	size.y = titleHeight + (m_margin * 4) + (rowHeight * numRows) + (rowSeparation * (numRows - 1));
	return size;
}

Vector2f InfoPanel::GetSize() const
{
	float labelColWidth = 150;
	float valueColWidth = 80;
	float visualColWidth = 60;
	float colWidth = labelColWidth + valueColWidth + visualColWidth;
	float rowHeight = 12;
	float rowSeparation = 2;
	float titleHeight = 20;
	int numRows = (int) m_rows.size();

	Vector2f size;
	size.x = colWidth + (m_margin * 2);
	size.y = titleHeight + (m_margin * 4) + (rowHeight * numRows) + (rowSeparation * (numRows - 1));
	return size;
}

void InfoPanel::Clear()
{
	m_rows.clear();
}

void InfoPanel::AddSeparator()
{
	// TODO: InfoPanel::AddSeparator()
	AddItem("---------------").SetValue("------");
}

InfoPanelItem& InfoPanel::AddItem(const std::string& label)
{
	m_rows.push_back(InfoPanelItem());
	InfoPanelItem& item = m_rows[m_rows.size() - 1];
	item.SetLabel(label);
	return item;
}

InfoPanelItem& InfoPanel::AddItem(const std::string& label, const std::string& valueSuffix)
{
	m_rows.push_back(InfoPanelItem());
	InfoPanelItem& item = m_rows[m_rows.size() - 1];
	item.SetLabel(label, valueSuffix);
	return item;
}

void InfoPanel::Draw(Graphics& g, const Rect2f& bounds)
{
	Vector2f position = bounds.position;

	int numRows = (int) m_rows.size();

	m_margin = 6;

	float totalColWidth = bounds.size.x - (m_margin * 2);
	float labelColWidth = 0.5f * totalColWidth;
	float valueColWidth = 0.3f * totalColWidth;
	float visualColWidth = 0.2f * totalColWidth;

	//float labelColWidth = 150;
	//float valueColWidth = 80;
	//float visualColWidth = 60;
	float colWidth = labelColWidth + valueColWidth + visualColWidth;
	float rowHeight = 12;
	float rowSeparation = 2;
	float titleMargin = 20;

	Vector2f rowHalfHeight(0.0f, rowHeight * 0.5f);

	Rect2f titleBox(position.x, position.y, colWidth, titleMargin);
	titleBox.size += Vector2f(m_margin * 2.0f);
	Rect2f contentBox(position.x, titleBox.GetBottom(), colWidth,
		(rowHeight * numRows) + (rowSeparation * (numRows - 1)));
	contentBox.size += Vector2f(m_margin * 2.0f);
	Rect2f panelBox = titleBox;
	panelBox.size.y += contentBox.size.y;
	Vector2f rowPos = contentBox.position + Vector2f((float) m_margin);
	
	// Fill panel background.
	g.FillRect(panelBox, Color::BLACK);

	// Draw rows.
	for (unsigned int i = 0; i < m_rows.size(); ++i)
	{
		InfoPanelItem& item = m_rows[i];
		
		Vector2f labelPos = rowPos;
		Vector2f valuePos = rowPos + Vector2f(labelColWidth, 0);
		Vector2f visualPos = valuePos + Vector2f(valueColWidth, 0);

		Rect2f rowBox(rowPos.x, rowPos.y, colWidth, rowHeight);
		if (i > 0)
			rowBox.Inflate(0, rowSeparation * 0.5f, 0, 0);
		if (i < m_rows.size() - 1)
			rowBox.Inflate(0, 0, 0, rowSeparation * 0.5f);

		Color rowColor = Color::BLACK;
		if (i % 2 == 1)
			rowColor = Color(24, 24, 24);

		// Draw row background.
		g.FillRect(rowBox, rowColor);

		// Draw label and value text.
		item.UpdateValueText();
		g.DrawString(m_font, item.m_labelText, labelPos + rowHalfHeight, item.m_labelColor, TextAlign::MIDDLE_LEFT);
		g.DrawString(m_font, item.m_valueText, valuePos + rowHalfHeight, item.m_valueColor, TextAlign::MIDDLE_LEFT);

		Rect2f visualBox(visualPos.x, visualPos.y, visualColWidth, rowHeight);

		// Draw visual.
		if (item.m_visualType == InfoPanelItem::VISUAL_BAR)
		{
			float valPos = (item.m_valueFloat - item.m_visualBarMinValue) /
				(item.m_visualBarMaxValue - item.m_visualBarMinValue);
			valPos = Math::Clamp(valPos, 0.0f, 1.0f);
			float centerPos = (item.m_visualBarCenterValue - item.m_visualBarMinValue) /
				(item.m_visualBarMaxValue - item.m_visualBarMinValue);
			centerPos = Math::Clamp(centerPos, 0.0f, 1.0f);
			
			Rect2f fillBox;
			fillBox.position.x = visualBox.position.x + (centerPos * visualBox.size.x);
			fillBox.position.y = visualBox.position.y;
			fillBox.size.x = visualBox.size.x * (valPos - centerPos);
			fillBox.size.y = visualBox.size.y;

			if (fillBox.size.x < 0.0f)
			{
				fillBox.size.x = -fillBox.size.x;
				fillBox.position.x -= fillBox.size.x;
			}

			// Draw bar filled box.
			g.FillRect(fillBox, item.m_visualColor);

			// Draw center line.
			if (centerPos > 0.0f && centerPos < 1.0f)
			{
				g.DrawLine(
					visualBox.position.x + (centerPos * visualBox.size.x),
					visualBox.position.y,
					visualBox.position.x + (centerPos * visualBox.size.x),
					visualBox.position.y + visualBox.size.y,
					Color::WHITE);
			}

			// Draw bar outline.
			g.DrawRect(visualBox, item.m_visualColor);
		}
		else if (item.m_visualType == InfoPanelItem::VISUAL_SOLID_COLOR)
		{
			g.FillRect(visualBox, item.m_visualColor);
		}

		rowPos.y += rowHeight + rowSeparation;
	}
	
	// Draw title.
	Vector2f titlePos = titleBox.GetCenter();
	g.DrawString(m_font, m_title, titlePos, Color::WHITE, TextAlign::CENTERED);

	// Draw panel borders.
	g.DrawRect(titleBox, Color::WHITE);
	g.DrawRect(contentBox, Color::WHITE);
}



