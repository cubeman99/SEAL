#ifndef _SHADER_H_
#define _SHADER_H_

#include "Color.h"
#include <string>
#include <vector>

#define MAX_SHADER_STAGES 4


// Data types for shader parameters.
struct UniformType
{
	typedef int value_type;

	enum
	{
		UNKNOWN = -1,
		TEXTURE = 0,
		INTEGER,
		VEC2,
		VEC3,
		VEC4,
		FLOAT,
		BOOLEAN,
		MATRIX,
		STRING,
	};
};


// Types of supported shaders stages.
struct ShaderType
{
	typedef int value_type;

	enum
	{
		VERTEX_SHADER = 0,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER,
		COMPUTE_SHADER,
		COUNT,
	};
};


class Uniform
{
public:
	friend class Shader;
	typedef UniformType::value_type uniform_type;

public:
	Uniform() :
		m_name(""),
		m_type(UniformType::UNKNOWN),
		m_location(-1),
		m_samplerSlot(0)
	{
	}

	// Accessors.
	inline const std::string&	GetName()			const { return m_name; }
	inline uniform_type			GetType()			const { return m_type; }
	inline int					GetLocation()		const { return m_location; }
	inline int					GetSamplerSlot()	const { return m_samplerSlot; }

private:
	std::string		m_name;
	uniform_type	m_type;
	int				m_location;
	int				m_samplerSlot;
};


class Shader
{
public:
	Shader();
	~Shader();
	
	// Accessors.
	//const char*		GetName() const;
	bool				IsLinked() const;
	unsigned int		GetNumUniforms() const;
	const Uniform&		GetUniform(unsigned int index) const;
	const Uniform*		GetUniform(const std::string& name) const;
			
	bool AddStage(const std::string& code, ShaderType::value_type type);
	bool CompileAndLink();

private:
public:
	void GenerateUniforms();

	unsigned int			m_glProgram;
	unsigned int			m_glShaderStages[MAX_SHADER_STAGES];
	bool					m_isLinked;
	std::vector<Uniform>	m_uniforms;
	//std::map<std::string, ShaderParam> m_paramMap;
};


#endif // _SHADER_H_