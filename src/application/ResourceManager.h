#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <graphics/Shader.h>
#include <graphics/Mesh.h>
#include <graphics/Material.h>
#include <graphics/Texture.h>
#include <utilities/FileUtility.h>
#include <map>


//-----------------------------------------------------------------------------
// ResourceManager - Handles resource loading, access, and memory management.
//                   Resources are mapped by a name string.
//-----------------------------------------------------------------------------
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	// Paths
	inline const std::string& GetAssetsPath() const { return m_assetsPath; }
	inline void SetAssetsPath(const std::string& assetsPath) { m_assetsPath = assetsPath; }

	// Resource loading
	Texture* LoadTexture(const std::string& name, const std::string& path);
	Shader* LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
	Mesh* LoadMesh(const std::string& name, const std::string& path);

	// Resource management
	void AddTexture(const std::string& name, Texture* texture);
	void AddMesh(const std::string& name, Mesh* mesh);
	void AddMaterial(const std::string& name, Material* material);
	void AddShader(const std::string& name, Shader* shader);

	// Resource access
	Texture* GetTexture(const std::string& name);
	Mesh* GetMesh(const std::string& name);
	Material* GetMaterial(const std::string& name);
	Shader* GetShader(const std::string& name);


private:
	typedef std::map<std::string, Texture*> TextureMap;
	typedef std::map<std::string, Mesh*> MeshMap;
	typedef std::map<std::string, Material*> MaterialMap;
	typedef std::map<std::string, Shader*> ShaderMap;

	TextureMap m_textures;
	MeshMap m_meshes;
	MaterialMap m_materials;
	ShaderMap m_shaders;
	std::string m_assetsPath;
};


#endif // _RESOURCE_MANAGER_H_