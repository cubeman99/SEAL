#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <graphics/Color.h>


class Material
{
public:
	Material();


	bool IsLit() const { return m_isLit; }
	const Color& GetColor() const { return m_color; }

	void SetColor(const Color& color) { m_color = color; }
	void SetIsLit(bool isLit) { m_isLit = isLit ; }

	
private:
	Color m_color;
	bool m_isLit;
};


#endif // _MATERIAL_H_