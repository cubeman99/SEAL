#ifndef _TEXTURE_PARAMS_H_
#define _TEXTURE_PARAMS_H_

#include <graphics/Color.h>
#include <graphics/ImageFormat.h>
	
	
//-----------------------------------------------------------------------------
// Modes for wrapping texture coordinates.
struct TextureWrap
{
	typedef int value_type;

	enum
	{
		CLAMP_TO_EDGE,			// The coordinate will be clamped between 0.0 and 1.0.
		CLAMP_TO_BORDER,		// The coordinates that fall outside the range will be given a specified border color.
		MIRROR_REPEAT,			// The texture will also be repeated, but it will be mirrored when the integer part of the coordinate is odd.
		REPEAT,					// The integer part of the coordinate will be ignored and a repeating pattern is formed.
		//MIRROR_CLAMP_TO_EDGE,
	};
};
	
//-----------------------------------------------------------------------------
// Modes for texture filtering.

// Texture filtering presets, each specifies the min, mag, and mipmap filters.
struct TextureFilter
{
	typedef int value_type;

	enum
	{
		NEAREST,		// min = NEAREST, mag = NEAREST, mipmap = NONE.
		BILINEAR,		// min = LINEAR,  mag = LINEAR,  mipmap = NEAREST.
		TRILINEAR,		// min = LINEAR,  mag = LINEAR,  mipmap = LINEAR.
		ANISOTROPIC,	// Anisotropic filtering on top of trilinear filtering.
	};
};

// Texture filtering modes.
struct TextureFilterOptions
{
	typedef int value_type;

	enum
	{
		NONE,			// No filtering (used to disable mipmap selection).
		NEAREST,		// Sample nearest texels.
		LINEAR,			// Linear interpolation with neighboring texels.
	};
};
	
//-----------------------------------------------------------------------------
// Bit flags for texture usage.
struct TextureUsage
{
	typedef int value_type;

	enum
	{
		STATIC							= 0x1,
		DYNAMIC							= 0x2,
		WRITE_ONLY						= 0x4,
		STATIC_WRITE_ONLY				= 0x8,
		DYNAMIC_WRITE_ONLY				= 0x10,
		DYNAMIC_WRITE_ONLY_DISCARDABLE	= 0x20,

		AUTO_MIPMAP						= 0x40,		// Automatically generate mipmaps.
		RENDER_TARGET					= 0x80,		// The texture is a render target.
		ANISOTROPIC						= 0x100,	// Use anisotropic filtering.

		DEFAULT = AUTO_MIPMAP | STATIC_WRITE_ONLY
	};
};
	
//-----------------------------------------------------------------------------
// The types of targets that a texture can be bound to.
struct TextureTarget
{
	typedef int value_type;

	enum
	{
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_CUBE_MAP,
		NUM_TEXTURE_TARGETS
	};
};
	
//-----------------------------------------------------------------------------
// Indices for cubemap faces.
struct CubeMapFace
{
	typedef int index_type;

	enum
	{
		POSITIVE_X = 0,
		NEGATIVE_X,
		POSITIVE_Y,
		NEGATIVE_Y,
		POSITIVE_Z,
		NEGATIVE_Z,
		NUM_CUBEMAP_FACES,
	};
};


// Flags:
// ANISOTROPIC
// AUTO_MIPMAP
// RENDER_TARGET
// 
// Misc:
// TEXTURE_FLAGS_PROCEDURAL
// TEXTURE_FLAGS_NORMAL				// The texture is a normal map.
// TEXTURE_FLAGS_RENDER_TARGET
// TEXTURE_FLAGS_DEPTHRENDERTARGET	// Depth render target.
// TEXTURE_FLAGS_NODEPTHBUFFER		// There is no depth buffer (for render targets)
// TEXTURE_FLAGS_NOMIPS				// Don't generate mipmaps.
// TEXTURE_FLAGS_TRANSLUCENT		// The texture is translucent (partially transparent).


//-----------------------------------------------------------------------------
// Texture Parameters
//-----------------------------------------------------------------------------

class TextureParams
{
public:
	typedef TextureTarget::value_type			texture_target_type;
	typedef TextureFilterOptions::value_type	texture_filter_options_type;
	typedef TextureWrap::value_type				texture_wrap_type;
	typedef PixelInternalFormat::value_type		pixel_internal_format_type;

public:
	TextureParams();

	//-----------------------------------------------------------------------------
	// Getters.

	texture_target_type			GetTarget()			const { return m_target; }
	texture_filter_options_type	GetMinFilter()		const { return m_minFilter; }
	texture_filter_options_type	GetMagFilter()		const { return m_magFilter; }
	texture_filter_options_type	GetMipMapFilter()	const { return m_mipFilter; }
	texture_wrap_type			GetWrapS()			const { return m_wrapS; }
	texture_wrap_type			GetWrapT()			const { return m_wrapT; }
	texture_wrap_type			GetWrapR()			const { return m_wrapR; }
	pixel_internal_format_type	GetInternalFormat()	const { return m_internalFormat; }
	bool						AutoGenMipMaps()	const { return m_autoGenMipMaps; }
	bool						UseAnisotropy()		const { return m_anisotropy; }

	//-----------------------------------------------------------------------------
	// Setters.

	void SetTarget(texture_target_type target)							{ m_target = target; }
	void SetMinFilter(texture_filter_options_type minFilter)			{ m_minFilter = minFilter; }
	void SetMagFilter(texture_filter_options_type magFilter)			{ m_magFilter = magFilter; }
	void SetMipMapFilter(texture_filter_options_type mipMapFilter)		{ m_mipFilter = mipMapFilter; }
	void SetWrapS(texture_wrap_type wrapS)								{ m_wrapS = wrapS; }
	void SetWrapT(texture_wrap_type wrapT)								{ m_wrapT = wrapT; }
	void SetWrapR(texture_wrap_type wrapR)								{ m_wrapR = wrapR; }
	void SetInternalFormat(pixel_internal_format_type internalFormat)	{ m_internalFormat = internalFormat; }
	void SetAutoGenMipMaps(bool autoGenMipMaps)							{ m_autoGenMipMaps = autoGenMipMaps; }
	void SetAnisotropy(bool anisotropy)									{ m_anisotropy = anisotropy; }

	void SetFiltering(texture_filter_options_type minFilter,
					  texture_filter_options_type magFilter,
					  texture_filter_options_type mipMapFilter);

	void SetFiltering(TextureFilter::value_type filtering);

	void SetWrap(texture_wrap_type wrap);

private:
	texture_target_type			m_target;
	texture_filter_options_type	m_minFilter;		// Minimize filter.
	texture_filter_options_type	m_magFilter;		// Magnify filter.
	texture_filter_options_type	m_mipFilter;		// Mipmap filter.
	texture_wrap_type			m_wrapS;
	texture_wrap_type			m_wrapT;
	texture_wrap_type			m_wrapR;

	pixel_internal_format_type	m_internalFormat;
	bool						m_autoGenMipMaps;	// Automatically generate mipmaps.
	bool						m_anisotropy;
};


#endif // _TEXTURE_PARAMS_H_