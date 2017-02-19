#ifndef _SPRITE_FONT_H_
#define _SPRITE_FONT_H_

#include <math/Vector2f.h>
#include <math/Vector4f.h>
#include "Texture.h"


class SpriteFont
{
public:
	friend class Graphics;

public:
	SpriteFont(Texture* texture, int charsPerRow, int charWidth, int charHeight, int charSpacing);
	~SpriteFont();

	inline Texture* GetTexture() const { return m_texture; }

private:
	Texture*	m_texture;
	int			m_charWidth;
	int			m_charHeight;
	int			m_charSpacing;
	int			m_charsPerRow;
};


typedef SpriteFont Font;


#endif // _SPRITE_FONT_H_