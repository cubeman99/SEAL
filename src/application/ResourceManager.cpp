#include "ResourceManager.h"
#include <wx/wx.h>
#include <utilities/Logging.h>


//-----------------------------------------------------------------------------
// Internal functions for generic resource management
//-----------------------------------------------------------------------------

// Retreive a resource from a resource map (or NULL if it doesn't exist).
template <class T>
static T* GetResource(std::map<std::string, T*>& map, const std::string& name)
{
	std::map<std::string, T*>::iterator it = map.find(name);
	if (it != map.end())
		return it->second;
	return nullptr;
}

// Add a resource to a resource map.
template <class T>
static void AddResource(std::map<std::string, T*>& map, const std::string& name, T* resource)
{
	assert(map.find(name) == map.end());
	map[name] = resource;
}

// Delete a resource from a resource map.
template <class T>
static void FreeResources(std::map<std::string, T*>& map)
{
	for (std::map<std::string, T*>::iterator it = map.begin(); it != map.end(); it++)
		delete it->second;
	map.clear();
}


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

ResourceManager::ResourceManager()
{
	m_assetsPath = "";
}

ResourceManager::~ResourceManager()
{
	FreeResources(m_textures);
	FreeResources(m_meshes);
	FreeResources(m_materials);
	FreeResources(m_shaders);
	FreeResources(m_fonts);
}


//-----------------------------------------------------------------------------
// Resource loading
//-----------------------------------------------------------------------------

Texture* ResourceManager::LoadTexture(const std::string& name, const std::string& path, const TextureParams& params)
{
	Texture* texture = Texture::LoadTexture(m_assetsPath + path, params);
	if (texture == nullptr)
		return nullptr;
	AddTexture(name, texture);
	return texture;
}

Texture* ResourceManager::LoadCubeMapTexture(const std::string& name, std::string paths[6], const TextureParams& params)
{
	std::string finalPaths[6];
	for (unsigned int i = 0; i < 6; ++i)
		finalPaths[i] = m_assetsPath + paths[i];
	Texture* texture = Texture::LoadCubeMap(finalPaths, params);
	if (texture == nullptr)
		return nullptr;
	AddTexture(name, texture);
	return texture;
}

Shader* ResourceManager::LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSource = "";
	std::string fragmentSource = "";
	bool vertexSuccess;
	bool fragmentSuccess;
	Shader* shader = new Shader();

	// Load the vertex shader source code.
	vertexSuccess = FileUtility::OpenAndGetContents(m_assetsPath + vertexPath, vertexSource);
	if (!vertexSuccess)
		SEAL_LOG_MSG("Error loading vertex shader '%s'", vertexPath.c_str());

	// Load the fragment shader source code.
	fragmentSuccess = FileUtility::OpenAndGetContents(m_assetsPath + fragmentPath, fragmentSource);
	if (!fragmentSuccess)
		SEAL_LOG_MSG("Error loading fragment shader '%s'", fragmentPath.c_str());
	
	// Create the shader.
	if (vertexSuccess && fragmentSuccess)
	{
		// Add the two shader stages.
		shader->AddStage(vertexSource, ShaderType::VERTEX_SHADER, vertexPath);
		shader->AddStage(fragmentSource, ShaderType::FRAGMENT_SHADER, fragmentPath);

		// Compile and link the shader.
		ShaderCompileError compileError;
		if (!shader->CompileAndLink(&compileError))
			SEAL_LOG_MSG(compileError.GetMessage().c_str());
	}

	AddShader(name, shader);
	return shader;
}

Mesh* ResourceManager::LoadMesh(const std::string& name, const std::string& path)
{
	std::string fileContents;

	// Read the file contents.
	if (!FileUtility::OpenAndGetContents(m_assetsPath + path, fileContents))
	{
		SEAL_LOG_MSG("Error loading mesh file '%s'", path.c_str());
		return nullptr;
	}

	// Import the mesh from the file contents.
	Mesh* mesh = Mesh::ImportFromOBJ(fileContents);
	if (mesh == nullptr)
	{
		SEAL_LOG_MSG("Error parsing .obj data from mesh file '%s'", path.c_str());
		return nullptr;
	}

	AddMesh(name, mesh);
	return mesh;
}

Font* ResourceManager::LoadSpriteFont(const std::string& name,
	const std::string& path, int charsPerRow, int charWidth,
	int charHeight, int charSpacing)
{
	// Load the font texture.
	Texture* texture = Texture::LoadTexture(m_assetsPath + path);
	if (texture == nullptr)
		return nullptr;

	// Create the sprite font.
	SpriteFont* font = new SpriteFont(texture,
		charsPerRow, charWidth, charHeight, charSpacing);
	AddFont(name, font);
	return font;
}


//-----------------------------------------------------------------------------
// Procedural resource creation
//-----------------------------------------------------------------------------

Mesh* ResourceManager::CreateMesh(const std::string& name)
{
	Mesh* mesh = new Mesh();
	AddMesh(name, mesh);
	return mesh;
}

Material* ResourceManager::CreateMaterial(const std::string& name)
{
	Material* material = new Material();
	AddMaterial(name, material);
	return material;
}

Shader* ResourceManager::CreateShader(const std::string& name)
{
	Shader* shader = new Shader();
	AddShader(name, shader);
	return shader;
}


//-----------------------------------------------------------------------------
// Resource management
//-----------------------------------------------------------------------------

void ResourceManager::AddTexture(const std::string& name, Texture* texture)
{
	AddResource(m_textures, name, texture);
}

void ResourceManager::AddMesh(const std::string& name, Mesh* mesh)
{
	AddResource(m_meshes, name, mesh);
}

void ResourceManager::AddMaterial(const std::string& name, Material* material)
{
	AddResource(m_materials, name, material);
}

void ResourceManager::AddShader(const std::string& name, Shader* shader)
{
	AddResource(m_shaders, name, shader);
}

void ResourceManager::AddFont(const std::string& name, Font* font)
{
	AddResource(m_fonts, name, font);
}


//-----------------------------------------------------------------------------
// Resource loading
//-----------------------------------------------------------------------------

Texture* ResourceManager::GetTexture(const std::string& name)
{
	return GetResource(m_textures, name);
}

Mesh* ResourceManager::GetMesh(const std::string& name)
{
	return GetResource(m_meshes, name);
}

Material* ResourceManager::GetMaterial(const std::string& name)
{
	return GetResource(m_materials, name);
}

Shader* ResourceManager::GetShader(const std::string& name)
{
	return GetResource(m_shaders, name);
}

Font* ResourceManager::GetFont(const std::string& name)
{
	return GetResource(m_fonts, name);
}

