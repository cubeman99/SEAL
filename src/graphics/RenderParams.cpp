#include "RenderParams.h"

RenderParams::RenderParams()
	: m_depthFunc(CompareFunction::LESS_EQUAL)
	, m_frontFace(FrontFace::CLOCKWISE)
	, m_cullFace(CullFace::BACK)
	, m_polygonMode(PolygonMode::FILL)
	, m_blendEnabled(false)
	, m_depthTestEnabled(false)
	, m_depthWriteEnabled(true)
	, m_nearFarClippingEnabled(false)
	, m_cullFaceEnabled(false)
	, m_lineSmoothEnabled(false)
	, m_polygonSmoothEnabled(false)
	, m_clearColor(Color::BLACK)
	, m_clearBits(ClearBits::COLOR_BUFFER_BIT | ClearBits::DEPTH_BUFFER_BIT)
	, m_blendFunc(BlendFunc::SOURCE_ALPHA, BlendFunc::ONE_MINUS_SOURCE_ALPHA)
{
}
	
	
void RenderParams::SetBlendFunction(BlendFunc::value_type source,
									BlendFunc::value_type destination)
{
	m_blendFunc.source = source;
	m_blendFunc.destination = destination;
}
