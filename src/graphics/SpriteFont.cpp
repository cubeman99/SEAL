#include "SpriteFont.h"
#include "lodepng/lodepng.h"


SpriteFont::SpriteFont(const char* imageFileName, int charsPerRow, int charWidth, int charHeight, int charSpacing)
	: m_charsPerRow(charsPerRow)
	, m_charWidth(charWidth)
	, m_charHeight(charHeight)
	, m_charSpacing(charSpacing)
	, m_imageWidth(0)
	, m_imageHeight(0)
{
	glGenTextures(1, &m_glTextureId);
	glBindTexture(GL_TEXTURE_2D, m_glTextureId);
	
	std::vector<unsigned char> image;
	unsigned int width;
	unsigned int height;
	unsigned error = lodepng::decode(image, width, height, imageFileName);

	//if there's an error, display it
	if (error)
	{
		std::cout << "Error loading font image file '" << imageFileName << "'" << std::endl;
		std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	else
	{
		m_imageWidth  = (int) width;
		m_imageHeight = (int) height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

SpriteFont::~SpriteFont()
{
	glDeleteTextures(1, &m_glTextureId);
}

void SpriteFont::DrawString(const char* text, const Vector2f& position, const Vector4f& color, float scale)
{
	Vector2f charSize(m_charWidth  / (float) m_imageWidth,
					  m_charHeight / (float) m_imageHeight);
		
	glBindTexture(GL_TEXTURE_2D, m_glTextureId);
	glBegin(GL_QUADS);
	glColor4fv(&color.x);
	
	Vector2f cursor = position;
	Vector2f screenCharSize = Vector2f(m_charWidth * scale, m_charHeight * scale);
	
	Vector2f texCoord;
	int row, col, x, y;

	for (const char* charPtr = text; *charPtr != NULL; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = position.x;
			cursor.y += m_charHeight * scale;
		}
		else
		{
			col = c % m_charsPerRow;
			row = c / m_charsPerRow;
			x   = col * (m_charWidth  + m_charSpacing);
			y   = row * (m_charHeight + m_charSpacing);
			texCoord.x = x / (float) m_imageWidth;
			texCoord.y = y / (float) m_imageHeight;

			glTexCoord2f(texCoord.x, texCoord.y);
			glVertex2f(cursor.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y + screenCharSize.y);
			glTexCoord2f(texCoord.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x, cursor.y + screenCharSize.y);

			cursor.x += m_charWidth * scale;
		}
	}

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
