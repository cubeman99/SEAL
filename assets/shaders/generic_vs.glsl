#version 330 core

layout(location = 0) in vec3 a_vertPos;
layout(location = 1) in vec3 a_vertNorm;
layout(location = 2) in vec4 a_vertColor;
//in vec2 a_vertTexCoord;

//out vec2 v_texCoord;
out vec3 v_normal;
out vec4 v_color;
out vec3 v_lightColor;

uniform mat4 u_mvp;
uniform mat4 u_model;

void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	
	//v_texCoord = a_vertTexCoord;
	v_normal = normalize((u_model * vec4(a_vertNorm, 0)).xyz);
	v_color = a_vertColor;
	
	vec3 lightDir = normalize(vec3(-1, -1, -1));
	float lightAmount = dot(-lightDir, v_normal);
	lightAmount = (lightAmount + 1) / 2;
	lightAmount = (lightAmount + 1) / 2;
	//lightAmount = max(0, lightAmount);
	
	v_lightColor = vec3(lightAmount);
	//v_lightColor = (n + vec3(1)) / 2;
}
