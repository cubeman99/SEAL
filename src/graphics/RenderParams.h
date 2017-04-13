#ifndef _RENDER_PARAMS_H_
#define _RENDER_PARAMS_H_

#include "Color.h"


//-----------------------------------------------------------------------------
// Compare Function (used for depth function)
//-----------------------------------------------------------------------------

struct CompareFunction
{
	typedef int value_type;

	enum
	{
		ALWAYS_PASS,
		ALWAYS_FAIL,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL,
		EQUAL,
		NOT_EQUAL,
	};
};

/*
struct DepthFunc
{
	typedef int value_type;

	enum
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
	};
};*/

//-----------------------------------------------------------------------------
// Blend Function
//-----------------------------------------------------------------------------

struct BlendFunc
{
	typedef int value_type;
		
	enum
	{
		ZERO,
		ONE,
		SOURCE_COLOR,
		ONE_MINUS_SOURCE_COLOR,
		DESTINATION_COLOR,
		ONE_MINUS_DESTINATION_COLOR,
		SOURCE_ALPHA,
		ONE_MINUS_SOURCE_ALPHA,
		DESTINATION_ALPHA,
		ONE_MINUS_DESTINATION_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA,
		SOURCE_ALPHA_SATURATE,
		SOURCE1_COLOR,
		ONE_MINUS_SOURCE1_COLOR,
		SOURCE1_ALPHA,
		ONE_MINUS_SOURCE1_ALPHA,
	};

	BlendFunc(value_type source, value_type destination)
		: source(source)
		, destination(destination)
	{ }

	value_type source;
	value_type destination;
};

//-----------------------------------------------------------------------------
// Clear Bits
//-----------------------------------------------------------------------------

struct ClearBits
{
	typedef int value_type;

	enum
	{
		COLOR_BUFFER_BIT	= 0x1,
		DEPTH_BUFFER_BIT	= 0x2,
		STENCIL_BUFFER_BIT	= 0x4,
	};
};

//-----------------------------------------------------------------------------
// Culling
//-----------------------------------------------------------------------------

// Winding order that defines the front of a face.
struct FrontFace
{
	typedef int value_type;
		
	enum
	{
		CLOCKWISE,
		COUNTER_CLOCKWISE,
	};
};

// Which side(s) of a face is culled.
struct CullFace
{
	typedef int value_type;
		
	enum
	{
		FRONT,
		BACK,
		FRONT_AND_BACK,
	};
};

//-----------------------------------------------------------------------------
// Polygon Mode
//-----------------------------------------------------------------------------

// Method for rasterizing polygon shapes.
struct PolygonMode
{
	typedef int value_type;
		
	enum
	{
		POINT,
		LINE,
		FILL,
	};
};


//-----------------------------------------------------------------------------
// Render Params
//-----------------------------------------------------------------------------

class RenderParams
{
public:
	RenderParams();

	//-----------------------------------------------------------------------------
	// Getters.

	CompareFunction::value_type	GetDepthFunction()	const { return m_depthFunc; }
	FrontFace::value_type		GetFrontFace()		const { return m_frontFace; }
	CullFace::value_type		GetCullFace()		const { return m_cullFace; }
	PolygonMode::value_type		GetPolygonMode()	const { return m_polygonMode; }
	ClearBits::value_type		GetClearBits()		const { return m_clearBits; }
	const BlendFunc&			GetBlendFunction()	const { return m_blendFunc; }
	const Color&				GetClearColor()		const { return m_clearColor; }
		
	bool IsBlendEnabled()					const {	return m_blendEnabled; }
	bool IsDepthTestEnabled()				const { return m_depthTestEnabled; }
	bool IsDepthBufferWriteEnabled()		const {	return m_depthWriteEnabled; }
	bool IsNearFarPlaneClippingEnabled()	const {	return m_nearFarClippingEnabled; }
	bool IsCullFaceEnabled()				const {	return m_cullFaceEnabled; }
	bool IsLineSmoothEnabled()				const {	return m_lineSmoothEnabled; }
	bool IsPolygonSmoothEnabled()			const { return m_polygonSmoothEnabled; }
		
	//-----------------------------------------------------------------------------
	// Setters.
		
	void SetDepthFunction(CompareFunction::value_type depthFunc)	{ m_depthFunc = depthFunc; }
	void SetFrontFace(FrontFace::value_type frontFace)				{ m_frontFace = frontFace; }
	void SetCullFace(CullFace::value_type cullFace)					{ m_cullFace = cullFace; }
	void SetPolygonMode(PolygonMode::value_type polygonMode)		{ m_polygonMode = polygonMode; }
	void SetClearBits(ClearBits::value_type clearBits)				{ m_clearBits = clearBits; }
	void SetClearColor(const Color& clearColor)						{ m_clearColor = clearColor; }
		
	void EnableBlend(bool enabled)					{ m_blendEnabled = enabled; }
	void EnableDepthTest(bool enabled)				{ m_depthTestEnabled = enabled; }
	void EnableDepthBufferWrite(bool enabled)		{ m_depthWriteEnabled = enabled; }
	void EnableNearFarPlaneClipping(bool enabled)	{ m_nearFarClippingEnabled = enabled; }
	void EnableCullFace(bool enabled)				{ m_cullFaceEnabled = enabled; }
	void EnableLineSmooth(bool enabled)				{ m_lineSmoothEnabled = enabled; }
	void EnablePolygonSmooth(bool enabled)			{ m_polygonSmoothEnabled = enabled; }

	void SetBlendFunction(BlendFunc::value_type source, BlendFunc::value_type destination);

		
private:
	CompareFunction::value_type	m_depthFunc;
	FrontFace::value_type		m_frontFace;
	CullFace::value_type		m_cullFace;
	PolygonMode::value_type		m_polygonMode;
	BlendFunc					m_blendFunc;
	ClearBits::value_type		m_clearBits;
	Color						m_clearColor;
	bool						m_blendEnabled;
	bool						m_depthTestEnabled;
	bool						m_depthWriteEnabled;
	bool						m_nearFarClippingEnabled;
	bool						m_cullFaceEnabled;
	bool						m_lineSmoothEnabled;
	bool						m_polygonSmoothEnabled;
};


#endif // _RENDER_PARAMS_H_