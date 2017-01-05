#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderParams.h"
#include <math/Matrix4f.h>
#include <math/Vector3f.h>
#include <math/Quaternion.h>
#include <math/Transform3f.h>
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
	void SetCamera(ICamera* camera);

	//void SetColor(const Color& color);
	//void SetColor(const Vector4f& color);
	//void SetColor(const Vector3f& color);
	void UpdateUniforms();

	void RenderMesh(Mesh* mesh, const Transform3f& transform);
	void RenderMesh(Mesh* mesh, const Matrix4f& modelMatrix);

private:
	RenderParams	m_params;
	Shader*			m_activeShader;
	ICamera*		m_camera;

	//Vector4f		m_color;
	//Matrix4f		m_modelMatrix;
	//Matrix4f		m_projectionMatrix;
	//Matrix4f		m_viewMatrix;
};


#endif // _RENDERER_H_