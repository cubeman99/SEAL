#include "ResourceManager.h"
#include <wx/wx.h>


template <class T>
static T* GetResource(std::map<std::string, T*>& map, const std::string& name)
{
	std::map<std::string, T*>::iterator it = map.find(name);
	if (it != map.end())
		return it->second;
	return nullptr;
}

template <class T>
static void AddResource(std::map<std::string, T*>& map, const std::string& name, T* resource)
{
	assert(map.find(name) == map.end());
	map[name] = resource;
}

template <class T>
static void FreeResources(std::map<std::string, T*>& map)
{
	for (std::map<std::string, T*>::iterator it = map.begin(); it != map.end(); it++)
		delete it->second;
	map.clear();
}

static bool OpenAndGetContents(const std::string& fileName, std::string& out)
{
	FILE* m_file = fopen(fileName.c_str(), "r");

	if (m_file == nullptr)
		return false;

	if (fseek(m_file, 0, SEEK_END) != 0)
		return false;

	long fileSize = ftell(m_file);
	if (fileSize < 0)
		return false;

	rewind(m_file);

	const size_t fileSizeInclNull = fileSize + 1;
	char* buffer = new char[fileSizeInclNull];

	size_t result = fread(buffer, 1, fileSizeInclNull, m_file);
	if (result == 0)
	{
		delete buffer;
		return false;
	}

	buffer[result] = '\0';
	out.assign(buffer, buffer + result);

	delete buffer;
	return true;
}



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
}

Texture* ResourceManager::LoadTexture(const std::string& name, const std::string& path)
{
	// TODO:
	assert(false);
	return nullptr;
}

Shader* ResourceManager::LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSource;
	std::string fragmentSource;
	
	if (!OpenAndGetContents(m_assetsPath + vertexPath, vertexSource))
	{
		wxMessageBox("Error loading vertex shader " + vertexPath, "Shader Error", wxICON_WARNING);
		return false;
	}
	if (!OpenAndGetContents(m_assetsPath + fragmentPath, fragmentSource))
	{
		wxMessageBox("Error loading fragment shader " + fragmentPath, "Shader Error", wxICON_WARNING);
		return false;
	}

	Shader* shader = new Shader();
	
	// Add the two stages.
	shader->AddStage(vertexSource, ShaderType::VERTEX_SHADER, vertexPath);
	shader->AddStage(fragmentSource, ShaderType::FRAGMENT_SHADER, fragmentPath);

	// Compile and link the shader.
	ShaderCompileError compileError;
	if (!shader->CompileAndLink(&compileError))
	{
		wxMessageBox(compileError.GetMessage(), "Shader Error", wxICON_WARNING);
		delete shader;
		return nullptr;
	}

	AddShader(name, shader);
	return shader;
}

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
