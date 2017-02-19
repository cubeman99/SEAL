#include "Graphics.h"
#include <math/MathLib.h>
#include "OpenGLIncludes.h"


Graphics::Graphics()
{
}


//-----------------------------------------------------------------------------
// General graphics operations
//-----------------------------------------------------------------------------

void Graphics::Clear(const Color& color)
{
	Vector4f c = color.ToVector4f();
	glClearColor(c.x, c.y, c.z, c.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::SetViewport(const Viewport& viewport, bool scissor)
{
	int y = viewport.y;
	glViewport(viewport.x, y, viewport.width, viewport.height);
	if (scissor)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(viewport.x, y, viewport.width, viewport.height);
	}
}


//-----------------------------------------------------------------------------
// Shape drawing
//-----------------------------------------------------------------------------

void Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& color)
{
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void Graphics::DrawLine(const Vector2f& from, const Vector2f& to, const Color& color)
{
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);
	glEnd();
}

void Graphics::DrawRect(const Viewport& rect, const Color& color)
{
	DrawRect((float) rect.x, (float) rect.y,
		(float) rect.width, (float) rect.height, color);
}

void Graphics::DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	DrawRect(pos.x, pos.y, size.x, size.y, color);
}

void Graphics::DrawRect(float x, float y, float width, float height, const Color& color)
{
	FillRect(x, y, 1, height, color);
	FillRect(x + width - 1, y, 1, height, color);
	FillRect(x, y, width, 1, color);
	FillRect(x, y + height - 1, width, 1, color);
}

void Graphics::FillRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	FillRect(pos.x, pos.y, size.x, size.y, color);
}

void Graphics::FillRect(const Viewport& rect, const Color& color)
{
	FillRect((float) rect.x, (float) rect.y,
		(float) rect.width, (float) rect.height, color);
}

void Graphics::FillRect(float x, float y, float width, float height, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}


void Graphics::DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v;
		v.x = pos.x + (Math::Cos(angle) * radius);
		v.y = pos.y + (Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
}

void Graphics::FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges)
{
	glBegin(GL_TRIANGLE_FAN);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
}


void Graphics::DrawString(const Font* font, const std::string& text, const Vector2f& position, const Color& color, int align)
{
	float scale = 1.0f;

	Texture* texture = font->GetTexture();
	Vector2f texSize((float) texture->GetWidth(), (float) texture->GetHeight());
	Vector2f charSize = Vector2f((float) font->m_charWidth , (float) font->m_charHeight) / texSize;
	Vector2f cursor = position;
	Vector2f screenCharSize = Vector2f(font->m_charWidth * scale, font->m_charHeight * scale);
	Vector2f texCoord;
	int row, col, x, y;

	// Change cursor position based on alignment.
	Vector2f stringSize = MeasureString(font, text);
	if (align & TextAlign::RIGHT)
		cursor.x -= stringSize.x;
	if (!(align & TextAlign::LEFT))
		cursor.x -= (int) (stringSize.x * 0.5f);
	if (align & TextAlign::BOTTOM)
		cursor.y -= stringSize.y;
	else if (!(align & TextAlign::TOP))
		cursor.y -= (int) (stringSize.y * 0.5f);

	glBindTexture(GL_TEXTURE_2D, font->GetTexture()->GetGLTextureId());
	glBegin(GL_QUADS);
	glColor4ubv(color.data());

	for (const char* charPtr = text.c_str(); *charPtr != '\0'; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = position.x;
			cursor.y += font->m_charHeight * scale;
		}
		else
		{
			col = c % font->m_charsPerRow;
			row = c / font->m_charsPerRow;
			x   = col * (font->m_charWidth  + font->m_charSpacing);
			y   = row * (font->m_charHeight + font->m_charSpacing);
			texCoord.x = x / (float) font->GetTexture()->GetWidth();
			texCoord.y = y / (float) font->GetTexture()->GetHeight();

			glTexCoord2f(texCoord.x, texCoord.y);
			glVertex2f(cursor.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y + screenCharSize.y);
			glTexCoord2f(texCoord.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x, cursor.y + screenCharSize.y);

			cursor.x += font->m_charWidth * scale;
		}
	}

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Vector2f Graphics::MeasureString(const Font* font, const std::string& text)
{
	Vector2f cursor(0, 0);
	Vector2f size(0, 0);

	for (const char* charPtr = text.c_str(); *charPtr != '\0'; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = 0.0f;
			cursor.y += font->m_charHeight;
		}
		else
		{
			cursor.x += font->m_charWidth;
		}

		size.x = Math::Max(size.x, cursor.x);
		size.y = Math::Max(size.y, cursor.y + font->m_charHeight);
	}

	return size;
}


//-----------------------------------------------------------------------------
// Render settings
//-----------------------------------------------------------------------------

void Graphics::EnableCull(bool cull)
{
	if (cull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void Graphics::EnableDepthTest(bool depthTest)
{
	if (depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}


//-----------------------------------------------------------------------------
// Projection
//-----------------------------------------------------------------------------

void Graphics::SetProjection(const Matrix4f& projection)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.data());
	glMatrixMode(GL_MODELVIEW);
}


//-----------------------------------------------------------------------------
// Transformations
//-----------------------------------------------------------------------------

void Graphics::ResetTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Graphics::SetTransform(const Matrix4f& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.data());
}

void Graphics::Transform(const Matrix4f& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(transform.data());
}

void Graphics::Rotate(const Vector3f& axis, float angle)
{
	glMatrixMode(GL_MODELVIEW);
	glRotatef(angle * Math::RAD_TO_DEG, axis.x, axis.y, axis.z);
}

void Graphics::Rotate(const Quaternion& rotation)
{
	Transform(Matrix4f::CreateRotation(rotation));
}

void Graphics::Translate(const Vector2f& translation)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translation.x, translation.y, 0.0f);
}

void Graphics::Translate(const Vector3f& translation)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translation.x, translation.y, translation.z);
}

void Graphics::Scale(float scale)
{
	glMatrixMode(GL_MODELVIEW);
	glScalef(scale, scale, scale);
}

void Graphics::Scale(const Vector3f& scale)
{
	glMatrixMode(GL_MODELVIEW);
	glScalef(scale.x, scale.y, scale.z);
}


//-----------------------------------------------------------------------------
// OpenGL wrappers
//-----------------------------------------------------------------------------

void Graphics::gl_Vertex(const Vector2f& v)
{
	glVertex2fv(v.data());
}

void Graphics::gl_Vertex(const Vector3f& v)
{
	glVertex3fv(v.data());
}

void Graphics::gl_Color(const Color& color)
{
	glColor4ubv(color.data());
}

