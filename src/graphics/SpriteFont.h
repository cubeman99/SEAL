#ifndef _SPRITE_FONT_H_
#define _SPRITE_FONT_H_

#include <Windows.h>
//#include <GL/glew.h>
#include <graphics/OpenGLIncludes.h>
#include <math/Vector2f.h>
#include <math/Vector4f.h>


class SpriteFont
{
public:
	SpriteFont(const char* imageFileName, int charsPerRow, int charWidth, int charHeight, int charSpacing);
	~SpriteFont();

	void DrawString(const char* text, const Vector2f& position, const Vector4f& color, float scale = 1.0f);

private:
	int		m_charWidth;
	int		m_charHeight;
	int		m_charSpacing;
	int		m_imageWidth;
	int		m_imageHeight;
	int		m_charsPerRow;
	GLuint	m_glTextureId;
};


#endif // _SPRITE_FONT_H_