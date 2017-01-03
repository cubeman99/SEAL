#include "TextureParams.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TextureParams::TextureParams()
	: m_target(TextureTarget::TEXTURE_2D)
		
	, m_minFilter(TextureFilterOptions::LINEAR) // Default is trilinear filtering.
	, m_magFilter(TextureFilterOptions::LINEAR)
	, m_mipFilter(TextureFilterOptions::LINEAR)
	, m_anisotropy(false)

	, m_wrapS(TextureWrap::CLAMP_TO_EDGE)
	, m_wrapT(TextureWrap::CLAMP_TO_EDGE)
	, m_wrapR(TextureWrap::CLAMP_TO_EDGE)

	, m_internalFormat(PixelInternalFormat::RGBA)
	, m_autoGenMipMaps(true)
{
}
	
//-----------------------------------------------------------------------------
// Mutators
//-----------------------------------------------------------------------------

void TextureParams::SetFiltering(TextureFilter::value_type filtering)
{
	if (filtering == TextureFilter::NEAREST)
	{
		m_minFilter = TextureFilterOptions::NEAREST;
		m_magFilter = TextureFilterOptions::NEAREST;
		m_mipFilter = TextureFilterOptions::NONE;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::BILINEAR)
	{
		m_minFilter = TextureFilterOptions::LINEAR;
		m_magFilter = TextureFilterOptions::LINEAR;
		m_mipFilter = TextureFilterOptions::NEAREST;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::TRILINEAR)
	{
		m_minFilter = TextureFilterOptions::LINEAR;
		m_magFilter = TextureFilterOptions::LINEAR;
		m_mipFilter = TextureFilterOptions::LINEAR;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::ANISOTROPIC)
	{
		m_minFilter = TextureFilterOptions::LINEAR;
		m_magFilter = TextureFilterOptions::LINEAR;
		m_mipFilter = TextureFilterOptions::LINEAR;
		m_anisotropy = true;
	}
}

void TextureParams::SetWrap(texture_wrap_type wrap)
{
	m_wrapS = wrap;
	m_wrapT = wrap;
	m_wrapR = wrap;
}
