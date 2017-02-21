#ifndef _INFO_PANEL_H_
#define _INFO_PANEL_H_

#include <string>
#include <vector>
#include <graphics/Graphics.h>


class InfoPanelItem
{
public:
	friend class InfoPanel;

public:
	enum
	{
		TYPE_STRING,
		TYPE_FLOAT,
		TYPE_INT,
	};
	enum
	{
		VISUAL_NONE,
		VISUAL_BAR,
		VISUAL_SOLID_COLOR,
	};

	InfoPanelItem();

	InfoPanelItem& SetLabel(const std::string& text);
	InfoPanelItem& SetLabelColor(const Color& color);

	InfoPanelItem& SetValue(const std::string& text);
	InfoPanelItem& SetValue(float value);
	InfoPanelItem& SetValue(int value);
	InfoPanelItem& SetValue(unsigned int value);
	InfoPanelItem& SetPrecision(int precision);
	InfoPanelItem& SetValueColor(const Color& color);

	InfoPanelItem& InitBar(const Color& color, float minValue, float maxValue, float centerValue = 0.0f);
	InfoPanelItem& InitSolidColor(const Color& color);
	InfoPanelItem& SetVisualColor(const Color& color);

	void UpdateValueText();

private:
	std::string	m_labelText;
	Color		m_labelColor;

	int			m_valueType;
	std::string m_valueText;
	float		m_valueFloat;
	int			m_valueInt;
	int			m_valuePrecision;
	Color		m_valueColor;

	int			m_visualType;
	Color		m_visualColor;
	float		m_visualBarMinValue;
	float		m_visualBarMaxValue;
	float		m_visualBarCenterValue;
};


class InfoPanel
{
public:
	InfoPanel();

	inline void SetTitle(const std::string& title) { m_title = title; }
	inline void SetFont(Font* font) { m_font = font; }

	Vector2f GetSize() const;

	InfoPanelItem& AddItem(const std::string& label);

	void Draw(Graphics& g, const Vector2f& position);

private:
	std::string	m_title;

	int			m_margin;
	int			m_width;
	int			m_height;
	Font*		m_font;

	std::vector<InfoPanelItem> m_rows;
};


#endif // _INFO_PANEL_H_