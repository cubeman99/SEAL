#include "SpriteFont.h"
#include "lodepng/lodepng.h"


SpriteFont::SpriteFont(Texture* texture, int charsPerRow,
		int charWidth, int charHeight, int charSpacing) :
	m_texture(texture),
	m_charsPerRow(charsPerRow),
	m_charWidth(charWidth),
	m_charHeight(charHeight),
	m_charSpacing(charSpacing)
{
}

SpriteFont::~SpriteFont()
{
	delete m_texture;
}
