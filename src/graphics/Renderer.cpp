#include "Renderer.h"
#include "OpenGLIncludes.h"


namespace
{
	void EnableDisableGL(bool enabled, GLenum cap)
	{
		if (enabled)
			glEnable(cap);
		else
			glDisable(cap);
	}

	GLenum TranslateCompareFunc(CompareFunction::value_type compareFunc)
	{
		switch (compareFunc)
		{
		case CompareFunction::ALWAYS_FAIL:		return GL_NEVER;
		case CompareFunction::LESS:				return GL_LESS;
		case CompareFunction::EQUAL:			return GL_EQUAL;
		case CompareFunction::LESS_EQUAL:		return GL_LEQUAL;
		case CompareFunction::GREATER:			return GL_GREATER;
		case CompareFunction::NOT_EQUAL:		return GL_NOTEQUAL;
		case CompareFunction::GREATER_EQUAL:	return GL_GEQUAL;
		case CompareFunction::ALWAYS_PASS:		return GL_ALWAYS;
		}
		return CompareFunction::LESS_EQUAL;
	}

	GLenum TranslateBlendFunc(BlendFunc::value_type blendFunc)
	{
		switch (blendFunc)
		{
		case BlendFunc::ZERO:							return GL_ZERO;
		case BlendFunc::ONE:							return GL_ONE;
		case BlendFunc::SOURCE_COLOR:					return GL_SRC_COLOR;
		case BlendFunc::ONE_MINUS_SOURCE_COLOR:			return GL_ONE_MINUS_SRC_COLOR;
		case BlendFunc::DESTINATION_COLOR:				return GL_DST_COLOR;
		case BlendFunc::ONE_MINUS_DESTINATION_COLOR:	return GL_ONE_MINUS_DST_COLOR;
		case BlendFunc::SOURCE_ALPHA:					return GL_SRC_ALPHA;
		case BlendFunc::ONE_MINUS_SOURCE_ALPHA:			return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFunc::DESTINATION_ALPHA:				return GL_DST_ALPHA;
		case BlendFunc::ONE_MINUS_DESTINATION_ALPHA:	return GL_ONE_MINUS_DST_ALPHA;
		case BlendFunc::CONSTANT_COLOR:					return GL_CONSTANT_COLOR;
		case BlendFunc::ONE_MINUS_CONSTANT_COLOR:		return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFunc::CONSTANT_ALPHA:					return GL_CONSTANT_ALPHA;
		case BlendFunc::ONE_MINUS_CONSTANT_ALPHA:		return GL_ONE_MINUS_CONSTANT_ALPHA;
		case BlendFunc::SOURCE_ALPHA_SATURATE:			return GL_SRC_ALPHA_SATURATE;
		case BlendFunc::SOURCE1_COLOR:					return GL_SRC1_COLOR;
		case BlendFunc::ONE_MINUS_SOURCE1_COLOR:		return GL_ONE_MINUS_SRC1_COLOR;
		case BlendFunc::SOURCE1_ALPHA:					return GL_SRC1_ALPHA;
		case BlendFunc::ONE_MINUS_SOURCE1_ALPHA:		return GL_ONE_MINUS_SRC1_ALPHA;
		}
		return GL_ONE;
	}

	GLbitfield TranslateClearBits(ClearBits::value_type clearBits)
	{
		GLbitfield returnClearBits = 0;
		if (clearBits & ClearBits::COLOR_BUFFER_BIT)
			returnClearBits |= GL_COLOR_BUFFER_BIT;
		if (clearBits & ClearBits::DEPTH_BUFFER_BIT)
			returnClearBits |= GL_DEPTH_BUFFER_BIT;
		if (clearBits & ClearBits::STENCIL_BUFFER_BIT)
			returnClearBits |= GL_STENCIL_BUFFER_BIT;
		return returnClearBits;
	}

	GLenum TranslateFrontFace(FrontFace::value_type frontFace)
	{
		if (frontFace == FrontFace::COUNTER_CLOCKWISE)
			return GL_CCW;
		else
			return GL_CW;
	}

	GLenum TranslateCullFace(CullFace::value_type cullFace)
	{
		if (cullFace == CullFace::BACK)
			return GL_BACK;
		else if (cullFace == CullFace::FRONT)
			return GL_FRONT;
		else
			return GL_FRONT_AND_BACK;
	}

	GLenum TranslatePolygonMode(PolygonMode::value_type polygonMode)
	{
		if (polygonMode == PolygonMode::FILL)
			return GL_FILL;
		else if (polygonMode == PolygonMode::LINE)
			return GL_LINE;
		else
			return GL_POINT;
	}

	GLenum TranslateVertexPrimitiveType(VertexPrimitiveType::value_type primitiveType)
	{
		switch (primitiveType)
		{
		case VertexPrimitiveType::TRIANGLES:		return GL_TRIANGLES;
		case VertexPrimitiveType::POINTS:			return GL_POINTS;
		case VertexPrimitiveType::LINES:			return GL_LINES;
		case VertexPrimitiveType::LINE_STRIP:		return GL_LINE_STRIP;
		case VertexPrimitiveType::LINE_LOOP:		return GL_LINE_LOOP;
		case VertexPrimitiveType::TRIANGLE_STRIP:	return GL_TRIANGLE_STRIP;
		case VertexPrimitiveType::TRIANGLE_FAN:		return GL_TRIANGLE_FAN;
		case VertexPrimitiveType::QUAD_STRIP:		return GL_QUAD_STRIP;
		case VertexPrimitiveType::POLYGON:			return GL_POLYGON;
		}
		return GL_TRIANGLES;
	}

};


Renderer::Renderer() :
	m_camera(nullptr),
	m_activeShader(nullptr),
	m_defaultShader(nullptr),
	m_isShaderChanged(false)
{
	m_lightDirection = Vector3f(-1, -1, -1);
	m_lightColor = Color::GRAY;
	m_ambientLight = Color::GRAY;
	
	m_lightColor = Color(255, 199, 0);
	m_ambientLight = Color(33, 36, 63);
}

	
void Renderer::SetShader(Shader* shader)
{
	m_activeShader = shader;
	m_isShaderChanged = true;

	if (m_activeShader != nullptr)
	{
		if (m_activeShader->IsLinked())
			glUseProgram(m_activeShader->m_glProgram);
		else if (m_defaultShader != nullptr && m_defaultShader->IsLinked())
		{
			m_activeShader = m_defaultShader;
			glUseProgram(m_defaultShader->m_glProgram);
		}
		else
			glUseProgram(0);
	}
	//else if (m_defaultShader != nullptr && m_defaultShader->IsLinked())
	//{
	//	m_activeShader = m_defaultShader;
	//	glUseProgram(m_defaultShader->m_glProgram);
	//}
	else
		glUseProgram(0);
}

void Renderer::SetDefaultShader(Shader* defaultShader)
{
	m_defaultShader = defaultShader;
}


void Renderer::SetCamera(ICamera* camera)
{
	m_camera = camera;
}

void Renderer::SetLightColor(const Color& lightColor)
{
	m_lightColor = lightColor;
}

void Renderer::SetAmbientLight(const Color& ambientLight)
{
	m_ambientLight = ambientLight;
}

void Renderer::SetLightDirection(const Vector3f& lightDir)
{
	m_lightDirection = lightDir;
}


void Renderer::ApplyRenderSettings(bool clear)
{
	// Depth.
	EnableDisableGL(m_params.IsDepthTestEnabled(), GL_DEPTH_TEST);
	EnableDisableGL(!m_params.IsNearFarPlaneClippingEnabled(), GL_DEPTH_CLAMP);
	glDepthFunc(TranslateCompareFunc(m_params.GetDepthFunction()));
	glDepthMask(m_params.IsDepthBufferWriteEnabled());				// glDepthMask(true) means writing to the depth buffer is enabled.

	// Face culling.
	EnableDisableGL(m_params.IsCullFaceEnabled(), GL_CULL_FACE);
	glFrontFace(TranslateFrontFace(m_params.GetFrontFace()));
	glCullFace(TranslateCullFace(m_params.GetCullFace()));
		
	// Smoothing.
	EnableDisableGL(m_params.IsLineSmoothEnabled(), GL_LINE_SMOOTH);
	EnableDisableGL(m_params.IsPolygonSmoothEnabled(), GL_POLYGON_SMOOTH);

	// Blend.
	EnableDisableGL(m_params.IsBlendEnabled(), GL_BLEND);
	glBlendFunc(TranslateBlendFunc(m_params.GetBlendFunction().source),
				TranslateBlendFunc(m_params.GetBlendFunction().destination));

	// Polygon mode.
	glPolygonMode(GL_FRONT_AND_BACK, TranslatePolygonMode(m_params.GetPolygonMode()));

	// Clear color.
	Vector4f clearColorVec = m_params.GetClearColor().ToVector4f();
	glClearColor(clearColorVec.x, clearColorVec.y,
				 clearColorVec.z, clearColorVec.w);
	
	if (clear)
		Clear();
}

void Renderer::Clear()
{
	glClear(TranslateClearBits(m_params.GetClearBits()));
}

//void Renderer::SetModelMatrix(const Matrix4f& model)
//{
//	m_modelMatrix = model;
//}
//
//void Renderer::SetViewMatrix(const Matrix4f& view)
//{
//	m_viewMatrix = view;
//}
//
//void Renderer::SetProjectionMatrix(const Matrix4f& projection)
//{
//	m_projectionMatrix = projection;
//}
//
//Matrix4f Renderer::GetMVP()
//{
//	return (m_projectionMatrix * m_viewMatrix * m_modelMatrix);
//}


void Renderer::UpdateUniforms(Material* material, const Matrix4f& modelMatrix)
{
	if (m_activeShader == nullptr)
		return;

	int uniformLocation = -1;
		
	// Material properties.
	//if (material != m_material || m_isShaderChanged)
	{
		m_material = material;
		if (m_activeShader->GetUniformLocation("u_isLit", uniformLocation))
			glUniform1i(uniformLocation, (int) material->IsLit());
		if (m_activeShader->GetUniformLocation("u_color", uniformLocation))
			glUniform4fv(uniformLocation, 1, material->GetColor().ToVector4f().data());
	}

	// Lighting properties.
	if (m_isShaderChanged)
	{
		if (m_activeShader->GetUniformLocation("u_lightColor", uniformLocation))
			glUniform3fv(uniformLocation, 1, m_lightColor.ToVector3f().data());
		if (m_activeShader->GetUniformLocation("u_ambientLight", uniformLocation))
			glUniform3fv(uniformLocation, 1, m_ambientLight.ToVector3f().data());
		if (m_activeShader->GetUniformLocation("u_lightDir", uniformLocation))
			glUniform3fv(uniformLocation, 1, m_lightDirection.data());
	}

	// Transform matrices.
	Matrix4f mvp = m_camera->GetViewProjection() * modelMatrix;
	if (m_activeShader->GetUniformLocation("u_mvp", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, mvp.data());
	if (m_activeShader->GetUniformLocation("u_model", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, modelMatrix.data());
		
	m_isShaderChanged = false;
}

void Renderer::RenderMesh(Mesh* mesh, Material* material, const Transform3f& transform)
{
	RenderMesh(mesh, material, transform.GetMatrix());
}

void Renderer::RenderMesh(Mesh* mesh, Material* material, const Matrix4f& modelMatrix)
{
	//glBindTexture(GL_TEXTURE_2D, 0);

	UpdateUniforms(material, modelMatrix * mesh->GetTransformMatrix());

	// Bind the mesh.
	if (m_mesh != mesh)
	{
		m_mesh = mesh;
		glBindVertexArray(mesh->GetVertexData()->GetVertexBuffer()->m_glVertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexData()->GetIndexBuffer()->m_glIndexBuffer);
	}
	
	// Send the draw call to the GPU.
	glDrawElements(TranslateVertexPrimitiveType(mesh->GetPrimitiveType()),
		mesh->GetIndexData()->GetCount(), GL_UNSIGNED_INT, (unsigned int*) 0);
}