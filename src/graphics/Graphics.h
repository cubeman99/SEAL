#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <math/Matrix4f.h>
#include <math/Rect2i.h>
#include <math/Vector2f.h>
#include <math/Vector3f.h>
#include <math/Vector4f.h>
#include <math/Quaternion.h>
#include <math/Rect2f.h>
#include "Color.h"
#include "SpriteFont.h"
#include <assert.h>
#include <string>


//-------------------------------------------------------------------------
// Viewport - integer rectangle class.
//-------------------------------------------------------------------------
struct Viewport
{
	int x;
	int y;
	int width;
	int height;

	Viewport()
	{}

	Viewport(int x, int y, int width, int height) :
		x(x), y(y), width(width), height(height)
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


//-------------------------------------------------------------------------
// TextAlign - bit flags for text alignment.
//-------------------------------------------------------------------------
struct TextAlign
{
	enum
	{
		CENTER	= 0x0,
		MIDDLE	= 0x0,
		TOP		= 0x1,
		BOTTOM	= 0x2,
		LEFT	= 0x4,
		RIGHT	= 0x8,

		TOP_LEFT		= TOP | LEFT,
		TOP_RIGHT		= TOP | RIGHT,
		TOP_CENTER		= TOP | CENTER,
		BOTTOM_LEFT		= BOTTOM | LEFT,
		BOTTOM_RIGHT	= BOTTOM | RIGHT,
		BOTTOM_CENTER	= BOTTOM | CENTER,
		MIDDLE_LEFT		= MIDDLE | LEFT,
		MIDDLE_RIGHT	= MIDDLE | RIGHT,
		CENTERED		= MIDDLE | CENTER,
	};
};


//-------------------------------------------------------------------------
// Graphics - Used for drawing 2D graphics.
//-------------------------------------------------------------------------
class Graphics
{
public:
	Graphics();

	//---------------------------------------------------------------------
	// General

	void Clear(const Color& color);

	void SetupCanvas2D(int width, int height);

	void SetCanvasSize(int width, int height);
	void SetViewportToCanvas();
	void SetViewport(const Rect2i& viewport, bool scissor);
	void SetViewport(const Viewport& viewport, bool scissor);

	//---------------------------------------------------------------------
	// Lines

	void DrawLine(float x1, float y1, float x2, float y2, const Color& color);
	void DrawLine(const Vector2f& from, const Vector2f& to, const Color& color);
	
	//---------------------------------------------------------------------
	// Rectangles

	void DrawRect(const Rect2f& rect, const Color& color);
	void DrawRect(const Viewport& rect, const Color& color);
	void DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void DrawRect(float x, float y, float width, float height, const Color& color);
	void FillRect(const Rect2f& rect, const Color& color);
	void FillRect(const Viewport& rect, const Color& color);
	void FillRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);
	
	//---------------------------------------------------------------------
	// Circles

	void DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);
	void FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);
	
	//---------------------------------------------------------------------
	// Text

	void DrawString(const Font* font, const std::string& text, float x, float y, const Color& color, int align = TextAlign::TOP_LEFT);
	void DrawString(const Font* font, const std::string& text, const Vector2f& position, const Color& color, int align = TextAlign::TOP_LEFT);
	Vector2f MeasureString(const Font* font, const std::string& text);

	//---------------------------------------------------------------------
	// Transformations

	void SetProjection(const Matrix4f& projection);
	void SetCanvasProjection();
	
	void ResetTransform();
	void SetTransform(const Matrix4f& transform);
	void Transform(const Matrix4f& transform);
	void Rotate(const Vector3f& axis, float angle);
	void Rotate(const Quaternion& rotation);
	void Translate(const Vector2f& translation);
	void Translate(const Vector3f& translation);
	void Scale(float scale);
	void Scale(const Vector3f& scale);
	
	
	void EnableCull(bool cull);
	void EnableDepthTest(bool depthTest);


private:
	
	void gl_Vertex(const Vector2f& v);
	void gl_Vertex(const Vector3f& v);
	void gl_Color(const Color& color);


	int m_canvasWidth;
	int m_canvasHeight;
};


#endif // _GRAPHICS_H_