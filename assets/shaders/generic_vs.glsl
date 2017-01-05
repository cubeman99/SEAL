#version 330 core

layout(location = 0) in vec3 a_vertPos;
layout(location = 1) in vec3 a_vertNorm;
layout(location = 2) in vec4 a_vertColor;
//in vec2 a_vertTexCoord;

//out vec2 v_texCoord;
//out vec3 v_normal;
out vec4 v_color;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	
	//v_texCoord = a_vertTexCoord;
	//v_normal = a_vertNorm;
	v_color = a_vertColor;
}
