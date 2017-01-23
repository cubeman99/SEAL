#include "Graphics.h"
#include <math/MathLib.h>
#include "OpenGLIncludes.h"


Graphics::Graphics()
{
}

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
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2i(rect.x, rect.y);
	glVertex2i(rect.x + rect.width, rect.y);
	glVertex2i(rect.x + rect.width, rect.y + rect.height);
	glVertex2i(rect.x, rect.y + rect.height);
	glEnd();
}

void Graphics::DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
}

void Graphics::DrawRect(float x, float y, float width, float height, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void Graphics::FillRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
}

void Graphics::FillRect(const Viewport& rect, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2i(rect.x, rect.y);
	glVertex2i(rect.x + rect.width, rect.y);
	glVertex2i(rect.x + rect.width, rect.y + rect.height);
	glVertex2i(rect.x, rect.y + rect.height);
	glEnd();
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
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
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

void Graphics::DrawString(SpriteFont* font, const char* text, const Vector2f& pos, const Color& color, float scale)
{
	font->DrawString(text, pos, color.ToVector4f(), scale);
}


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

void Graphics::SetProjection(const Matrix4f& projection)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.data());
	glMatrixMode(GL_MODELVIEW);
}

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
