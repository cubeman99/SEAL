#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <graphics/Color.h>
#include <graphics/Texture.h>


//-----------------------------------------------------------------------------
// Material
//-----------------------------------------------------------------------------
class Material
{
public:
	Material();

	// Getters
	bool IsLit() const { return m_isLit; }
	const Color& GetColor() const { return m_color; }
	Texture* GetTexture() const { return m_texture; }

	// Setters
	void SetColor(const Color& color) { m_color = color; }
	void SetIsLit(bool isLit) { m_isLit = isLit ; }
	void SetTexture(Texture* texture) { m_texture = texture ; }

	
private:
	Color		m_color;
	bool		m_isLit;
	Texture*	m_texture;
};


#endif // _MATERIAL_H_