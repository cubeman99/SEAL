#include "Texture.h"
#include <graphics/lodepng/lodepng.h>
#include <graphics/OpenGLIncludes.h>
#include <math/MathLib.h>
#include <vector>
#include <fstream>


//-----------------------------------------------------------------------------
// OpenGL Enum Conversions
//-----------------------------------------------------------------------------
namespace
{
	GLenum TranslateTextureTarget(TextureTarget::value_type target)
	{
		switch (target)
		{
		case TextureTarget::TEXTURE_2D:
			return GL_TEXTURE_2D;
		case TextureTarget::TEXTURE_3D:
			return GL_TEXTURE_3D;
		case TextureTarget::TEXTURE_1D:
			return GL_TEXTURE_1D;
		case TextureTarget::TEXTURE_CUBE_MAP:
			return GL_TEXTURE_CUBE_MAP;
		}
		return GL_TEXTURE_2D;
	}

	GLenum TranslateTextureWrap(TextureWrap::value_type wrap)
	{
		switch (wrap)
		{
		case TextureWrap::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		case TextureWrap::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureWrap::REPEAT:
			return GL_REPEAT;
		case TextureWrap::MIRROR_REPEAT:
			return GL_MIRRORED_REPEAT;
		//case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			//return GL_MIRROR_CLAMP_TO_EDGE; //TODO: Figure this out.
		}
		return GL_CLAMP_TO_BORDER;
	}

	GLenum TranslateMinMagFilter(TextureFilterOptions::value_type minMagFilter,
									TextureFilterOptions::value_type mipmapFilter)
	{
		//CMG_ASSERT(minMagFilter != TextureFilterOptions::NONE,
			//"You must specify a min and mag filter!");

		if (minMagFilter == TextureFilterOptions::LINEAR)
		{
			if (mipmapFilter == TextureFilterOptions::NONE)
				return GL_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::LINEAR)
				return GL_LINEAR_MIPMAP_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::NEAREST)
				return GL_LINEAR_MIPMAP_NEAREST;
		}
		else if (minMagFilter == TextureFilterOptions::NEAREST)
		{
			if (mipmapFilter == TextureFilterOptions::NONE)
				return GL_NEAREST;
			else if (mipmapFilter == TextureFilterOptions::LINEAR)
				return GL_NEAREST_MIPMAP_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::NEAREST)
				return GL_NEAREST_MIPMAP_NEAREST;
		}

		return GL_LINEAR;
	}

	GLenum GetGLPixelTransferFormat(PixelTransferFormat::value_type format)
	{
		switch (format)
		{
		case PixelTransferFormat::RED:
			return GL_RED;
		case PixelTransferFormat::GREEN:
			return GL_GREEN;
		case PixelTransferFormat::BLUE:
			return GL_BLUE;
		case PixelTransferFormat::RG:
			return GL_RG;
		case PixelTransferFormat::RGB:
			return GL_RGB;
		case PixelTransferFormat::BGR:
			return GL_BGR;
		case PixelTransferFormat::RGBA:
			return GL_RGBA;
		case PixelTransferFormat::BGRA:
			return GL_BGRA;
		case PixelTransferFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		case PixelTransferFormat::GREEN_INTEGER:
			return GL_GREEN_INTEGER;
		case PixelTransferFormat::BLUE_INTEGER:
			return GL_BLUE_INTEGER;
		case PixelTransferFormat::RG_INTEGER:
			return GL_RG_INTEGER;
		case PixelTransferFormat::RGB_INTEGER:
			return GL_RGB_INTEGER;
		case PixelTransferFormat::BGR_INTEGER:
			return GL_BGR_INTEGER;
		case PixelTransferFormat::RGBA_INTEGER:
			return GL_RGBA_INTEGER;
		case PixelTransferFormat::BGRA_INTEGER:
			return GL_BGRA_INTEGER;
		case PixelTransferFormat::DEPTH_COMPONENT:
			return GL_DEPTH_COMPONENT;
		case PixelTransferFormat::STENCIL_INDEX:
			return GL_STENCIL_INDEX;
		case PixelTransferFormat::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL;
		}
		return GL_RGBA;
	}

	GLenum GetGLInternalFormat(PixelInternalFormat::value_type format)
	{
		switch (format)
		{
		case PixelInternalFormat::RED:
			return GL_RED;
		case PixelInternalFormat::RG:
			return GL_RG;
		case PixelInternalFormat::RGB:
			return GL_RGB;
		case PixelInternalFormat::RGBA:
			return GL_RGBA;
		case PixelInternalFormat::DEPTH_COMPONENT:
			return GL_DEPTH_COMPONENT;
		case PixelInternalFormat::STENCIL_INDEX:
			return GL_STENCIL_INDEX;
		case PixelInternalFormat::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL;
		}
		return GL_RGBA;
	}

	GLenum GetGLPixelType(PixelType::value_type type)
	{
		switch (type)
		{
		case PixelType::TYPE_UNSIGNED_BYTE:
			return GL_UNSIGNED_BYTE;
		case PixelType::TYPE_BYTE:
			return GL_BYTE;
		case PixelType::TYPE_UNSIGNED_SHORT:
			return GL_UNSIGNED_SHORT;
		case PixelType::TYPE_SHORT:
			return GL_SHORT;
		case PixelType::TYPE_UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		case PixelType::TYPE_INT:
			return GL_INT;
		case PixelType::TYPE_FLOAT:
			return GL_FLOAT;
		}
		return GL_UNSIGNED_BYTE;
	}

	GLenum GetGLCubeMapFaceTarget(CubeMapFace::index_type faceIndex)
	{
		switch (faceIndex)
		{
		case CubeMapFace::POSITIVE_X:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case CubeMapFace::NEGATIVE_X:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case CubeMapFace::POSITIVE_Y:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case CubeMapFace::NEGATIVE_Y:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case CubeMapFace::POSITIVE_Z:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case CubeMapFace::NEGATIVE_Z:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		}
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}
};

	
//-----------------------------------------------------------------------------
// Constructor & destructor.
//-----------------------------------------------------------------------------

Texture::Texture()
	: m_width(0)
	, m_height(0)
	, m_depth(0)
	, m_numMipMaps(0)
	, m_anisotropyAmount(0.0f)
	, m_glTextureId(0)
	, m_glDepthBufferId(0)
	, m_glFrameBufferId(0)
{
	glGenTextures(1, &m_glTextureId);
}

Texture::Texture(const TextureParams& params)
	: Texture()
{
	m_params = params;
}

Texture::~Texture()
{
	if (m_glDepthBufferId != 0)
		glDeleteFramebuffers(1, &m_glFrameBufferId);
	if (m_glDepthBufferId != 0)
		glDeleteRenderbuffers(1, &m_glDepthBufferId);
	if (m_glTextureId != 0)
		glDeleteTextures(1, &m_glTextureId);
}



//-----------------------------------------------------------------------------
// Dimensions
//-----------------------------------------------------------------------------

int Texture::GetWidth() const
{
	return m_width;
}

int Texture::GetHeight() const
{
	return m_height;
}

int Texture::GetDepth() const
{
	return m_depth;
}


//-----------------------------------------------------------------------------
// Pixel Read & Write
//-----------------------------------------------------------------------------
	
//-----------------------------------------------------------------------------
// Pixel Read

// Store the texture's pixel data in the provided color map.
// The pixels will be stored in left-to-right, top-to-bottom order.
void Texture::ReadPixels(Color* pixels) const
{
	ReadPixels(PixelTransferFormat::RGBA, PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

// Store the texture's pixel data in the provided pixel map in a specified
// format. The pixels will be stored in left-to-right, top-to-bottom order.
void Texture::ReadPixels(pixel_format format, pixel_type type,
							void* pixels) const
{
	ReadPixels(0, format, type, pixels);
}

// Store the texture's pixel data in the provided color map.
// The pixels will be stored in left-to-right, top-to-bottom order.
void Texture::ReadPixels(int mipmapLevel, Color* pixels) const
{
	ReadPixels(mipmapLevel, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

// Store the texture's pixel data in the provided pixel map in a specified
// format. The pixels will be stored in left-to-right, top-to-bottom order.
void Texture::ReadPixels(int mipmapLevel, pixel_format format,
							pixel_type type, void* pixels) const
{
	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);
	glGetTexImage(target, mipmapLevel,
		GetGLPixelTransferFormat(format),
		GetGLPixelType(type), pixels);
}


//-----------------------------------------------------------------------------
// Cubemap Face Write

void Texture::WriteFacePixels(cubemap_face_index face, int width,
								int height, const Color* pixels)
{
	WriteFacePixelsAtLevel(face, 0, width, height,
		PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
	//if (m_params.AutoGenMipMaps())
	//	GenerateMipMaps();
}

void Texture::WriteFacePixels(cubemap_face_index face, int width,
								int height, pixel_format format,
								pixel_type type, const void* pixelData)
{
	WriteFacePixelsAtLevel(face, 0, width, height,
		format, type, pixelData);
	//if (m_params.AutoGenMipMaps())
	//	GenerateMipMaps();
}

void Texture::WriteFacePixelsAtLevel(cubemap_face_index face, int mipmapLevel,
								int width, int height, const Color* pixels)
{
	WriteFacePixelsAtLevel(face, mipmapLevel, width, height,
		PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

void Texture::WriteFacePixelsAtLevel(cubemap_face_index face, int mipmapLevel,
								int width, int height, pixel_format format,
								pixel_type type, const void* pixelData)
{
	//CMG_ASSERT(m_params.GetTarget() == TextureTarget::TEXTURE_CUBE_MAP,
		//"Texture must be a cubemap to modify face pixels.");
	//CMG_ASSERT(width == height, "Cube map face images must have a square size");

	if (mipmapLevel == 0)
	{
		m_width  = width;
		m_height = height;
	}

	GLenum target		= GetGLTextureTarget();
	GLenum faceTarget	= GetGLCubeMapFaceTarget(face);
	glBindTexture(target, m_glTextureId);
	glTexImage2D(faceTarget, mipmapLevel,
		GetGLInternalFormat(m_params.GetInternalFormat()),
		width, height, 0, GetGLPixelTransferFormat(format),
		GetGLPixelType(type), pixelData);
}

	
//-----------------------------------------------------------------------------
// 1D Texture Write

void Texture::WritePixels1D(int width, const Color* pixels)
{
	WritePixelsAtLevel1D(0, width, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixels1D(int width, pixel_format format,
							pixel_type type, const void* pixelData)
{
	WritePixelsAtLevel1D(0, width, format, type, pixelData);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixelsAtLevel1D(int mipmapLevel, int width, const Color* pixels)
{
	WritePixelsAtLevel1D(mipmapLevel, width, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

void Texture::WritePixelsAtLevel1D(int mipmapLevel, int width, pixel_format format,
							pixel_type type, const void* pixelData)
{
	//CMG_ASSERT(m_params.GetTarget() == TextureTarget::TEXTURE_1D,
		//"Texture target must be 1D to modify pixels like this.");

	if (mipmapLevel == 0)
	{
		m_width  = width;
	}

	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);
	glTexImage1D(target, mipmapLevel,
		GetGLInternalFormat(m_params.GetInternalFormat()),
		width, 0, GetGLPixelTransferFormat(format),
		GetGLPixelType(type), pixelData);
}

	
//-----------------------------------------------------------------------------
// 2D Texture Write

void Texture::WritePixels2D(int width, int height, const Color* pixels)
{
	WritePixelsAtLevel2D(0, width, height, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixels2D(int width, int height, pixel_format format,
							pixel_type type, const void* pixelData)
{
	WritePixelsAtLevel2D(0, width, height, format, type, pixelData);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixelsAtLevel2D(int mipmapLevel, int width, int height,
							const Color* pixels)
{
	WritePixelsAtLevel2D(mipmapLevel, width, height,
		PixelTransferFormat::RGBA, PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

void Texture::WritePixelsAtLevel2D(int mipmapLevel, int width, int height,
							pixel_format format, pixel_type type,
							const void* pixelData)
{
	//CMG_ASSERT(m_params.GetTarget() == TextureTarget::TEXTURE_2D,
		//"Texture target must be 2D to modify pixels like this.");

	if (mipmapLevel == 0)
	{
		m_width  = width;
		m_height = height;
	}

	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);
	glTexImage2D(target, mipmapLevel, 
		GetGLInternalFormat(m_params.GetInternalFormat()),
		width, height, 0, GetGLPixelTransferFormat(format),
		GetGLPixelType(type), pixelData);
}

	
//-----------------------------------------------------------------------------
// 3D Texture Write

void Texture::WritePixels3D(int width, int height, int depth, const Color* pixels)
{
	WritePixelsAtLevel3D(0, width, height, depth,
		PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixels3D(int width, int height, int depth,
							pixel_format format, pixel_type type,
							const void* pixelData)
{
	WritePixelsAtLevel3D(0, width, height, depth, format, type, pixelData);
	if (m_params.AutoGenMipMaps())
		GenerateMipMaps();
}

void Texture::WritePixelsAtLevel3D(int mipmapLevel, int width, int height,
							int depth, const Color* pixels)
{
	WritePixelsAtLevel3D(mipmapLevel, width, height, depth,
		PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, pixels);
}

void Texture::WritePixelsAtLevel3D(int mipmapLevel, int width, int height,
							int depth, pixel_format format,
							pixel_type type,
							const void* pixelData)
{
	//CMG_ASSERT(m_params.GetTarget() == TextureTarget::TEXTURE_3D,
		//"Texture target must be 3D to modify pixels like this.");

	if (mipmapLevel == 0)
	{
		m_width  = width;
		m_height = height;
		m_depth  = depth;
	}

	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);
	glTexImage3D(target, mipmapLevel,
		GetGLInternalFormat(m_params.GetInternalFormat()),
		width, height, depth, 0, GetGLPixelTransferFormat(format),
		GetGLPixelType(type), pixelData);
}


//-----------------------------------------------------------------------------
// Mipmaps
//-----------------------------------------------------------------------------

// Generate mipmaps for the texture.
void Texture::GenerateMipMaps()
{
	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);
	glGenerateMipmap(target);
	int size = Math::Max(m_width, m_height);
	m_numMipMaps = (int) Math::Log2((float) size) + 1;
}
	

//-----------------------------------------------------------------------------
// Texture Parameters
//-----------------------------------------------------------------------------
	
const TextureParams& Texture::GetParams() const
{
	return m_params;
}

void Texture::SetParams(const TextureParams& params)
{
	m_params = params;

	// Target.
	GLenum target = GetGLTextureTarget();
	glBindTexture(target, m_glTextureId);

	// Clamping.
	glTexParameteri(target, GL_TEXTURE_WRAP_S,
		TranslateTextureWrap(m_params.GetWrapS()));
	glTexParameteri(target, GL_TEXTURE_WRAP_T,
		TranslateTextureWrap(m_params.GetWrapT()));
	glTexParameteri(target, GL_TEXTURE_WRAP_R,
		TranslateTextureWrap(m_params.GetWrapR()));
		
	// Min-Mag Filtering.
	auto minFilter = m_params.GetMinFilter();
	auto magFilter = m_params.GetMagFilter();
	auto mipFilter = m_params.GetMipMapFilter();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
		TranslateMinMagFilter(minFilter, mipFilter));
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER,
		TranslateMinMagFilter(magFilter, mipFilter));

	// Anisotropic filtering.
	if (m_params.UseAnisotropy())
	{
		// TODO: Customize anisotropy samples.
		// Determine hardware's max anisotropy samples.
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_anisotropyAmount);
		m_anisotropyAmount = Math::Clamp(m_anisotropyAmount, 0.0f, 8.0f);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_anisotropyAmount);
	}
	else
	{
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
	}
}

	
//-----------------------------------------------------------------------------
// Factory functions.
//-----------------------------------------------------------------------------
	
void Texture::InitRenderTarget()
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &m_glFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	
	// Setup the depth buffer.
	glGenRenderbuffers(1, &m_glDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, m_glDepthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_glDepthBufferId);

	// Create the empty texture for rendering to.
	//glBindTexture(GL_TEXTURE_2D, pTexture->m_glTextureId);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	// Set the texture as our color attachement #0.
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pTexture->m_glTextureId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glTextureId, 0);
	
	// Set the list of draw buffers.
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	// Check that our framebuffer is ok.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

Texture* Texture::LoadTexture(const std::string& fileName, const TextureParams& params)
{
	// Open the file.
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cerr << "Error opening texture file " << fileName << std::endl;
		return NULL;
	}

	// Read it's entirety.
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> fileData((unsigned int) fileSize);
	if (!file.read(fileData.data(), fileSize))
	{
		std::cerr << "Error reading texture file " << fileName << std::endl;
		return NULL;
	}

	// Decode the image file into pixel data.
	unsigned int width;
	unsigned int height;
	std::vector<unsigned char> imageData;
	lodepng::State lodePngState;
	lodepng::decode(imageData, width, height, lodePngState,
		(unsigned char*) fileData.data(), fileData.size());
	if (lodePngState.error != 0)
	{
		std::cerr << "lodepng_decode32 failed: "
			<< lodepng_error_text(lodePngState.error) << std::endl;
		return NULL;
	}

	// Create the texture.
	TextureParams texParams = params;
	texParams.SetTarget(TextureTarget::TEXTURE_2D);
	Texture* texture = new Texture();
	texture->SetParams(texParams);
	texture->WritePixels2D(width, height,
		PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE,
		imageData.data());

	return texture;
}


//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------
	
void Texture::DoBindTexture()
{
	glBindTexture(GetGLTextureTarget(), m_glTextureId);
}

void Texture::DoUnbindTexture()
{
	glBindTexture(GetGLTextureTarget(), 0);
}

unsigned int Texture::GetGLTextureTarget() const
{
	return TranslateTextureTarget(m_params.GetTarget());
}
