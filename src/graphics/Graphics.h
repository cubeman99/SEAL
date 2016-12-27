#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <math/Vector2f.h>
#include <math/Vector3f.h>
#include <math/Vector4f.h>
#include <math/Matrix4f.h>
#include <math/Quaternion.h>
#include "Color.h"
//#include "Window.h"
#include "SpriteFont.h"
#include <assert.h>


struct Viewport
{
	int x;
	int y;
	int width;
	int height;

	Viewport()
	{}

	Viewport(int x, int y, int width, int height)
		: x(x), y(y), width(width), height(height)
	{}

	float GetAspectRatio() const
	{
		assert(height != 0);
		return ((float) width / (float) height);
	}

	void Inset(int amount)
	{
		x += amount;
		y += amount;
		width  -= amount * 2;
		height -= amount * 2;
	}

	void Inset(int left, int top, int right, int bottom)
	{
		x += left;
		y += top;
		width -= left + right;
		height -= top + bottom;
	}

	bool Contains(int x, int y)
	{
		return (x >= this->x && y >= this->y &&
				x < this->x + this->width &&
				y < this->y + this->height);
	}
};


class Graphics
{
public:
	Graphics();

	void Clear(const Color& color);
	void SetViewport(const Viewport& viewport, bool scissor);

	void DrawLine(float x1, float y1, float x2, float y2, const Color& color);
	void DrawLine(const Vector2f& from, const Vector2f& to, const Color& color);
	void DrawRect(const Viewport& rect, const Color& color);
	void DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void DrawRect(float x, float y, float width, float height, const Color& color);
	void FillRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void FillRect(const Viewport& rect, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);
	void FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);
	void DrawString(SpriteFont* font, const char* text, const Vector2f& pos, const Color& color, float scale = 1.0f);

	
	void EnableCull(bool cull);
	void EnableDepthTest(bool depthTest);

	void SetProjection(const Matrix4f& projection);
	
	void ResetTransform();
	void SetTransform(const Matrix4f& transform);
	
	void Transform(const Matrix4f& transform);
	void Rotate(const Vector3f& axis, float angle);
	void Rotate(const Quaternion& rotation);
	void Translate(const Vector2f& translation);
	void Translate(const Vector3f& translation);
	void Scale(float scale);
	void Scale(const Vector3f& scale);

private:
	
	void gl_Vertex(const Vector2f& v);
	void gl_Vertex(const Vector3f& v);
	void gl_Color(const Color& color);
};


#endif // _GRAPHICS_H_