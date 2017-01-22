#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderParams.h"
#include <math/Matrix4f.h>
#include <math/Vector3f.h>
#include <math/Quaternion.h>
#include <math/Transform3f.h>
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "ICamera.h"


class Renderer
{
public:
	Renderer();

	const RenderParams& GetRenderParams() const { return m_params; }
	
	void SetRenderParams(const RenderParams& renderParams) { m_params = renderParams; }

	void ApplyRenderSettings(bool clear = false);
	void Clear();

	//void SetModelMatrix(const Matrix4f& model);
	//void SetViewMatrix(const Matrix4f& view);
	//void SetProjectionMatrix(const Matrix4f& projection);
	//Matrix4f GetMVP();
	
	inline Shader* GetShader() { return m_activeShader; }
	inline ICamera* GetCamera() { return m_camera; }

	void SetShader(Shader* shader);
	void SetDefaultShader(Shader* defaultShader);
	void SetCamera(ICamera* camera);

	void SetLightColor(const Color& lightColor);
	void SetAmbientLight(const Color& ambientLight);
	void SetLightDirection(const Vector3f& lightDir);

	void UpdateUniforms(Material* material, const Matrix4f& modelMatrix);
	void RenderMesh(Mesh* mesh, Material* material, const Transform3f& transform);
	void RenderMesh(Mesh* mesh, Material* material, const Matrix4f& modelMatrix);

private:
	RenderParams	m_params;
	Shader*			m_activeShader;
	ICamera*		m_camera;
	
	Vector3f		m_lightDirection;
	Color			m_lightColor;
	Color			m_ambientLight;

	Shader*			m_defaultShader;

	Mesh*			m_mesh;
	Material*		m_material;

	bool			m_isMeshChanged;
	bool			m_isMaterialChanged;
	bool			m_isShaderChanged;
};


#endif // _RENDERER_H_