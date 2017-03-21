#version 330 core

layout (location = 0) in vec3 a_vertPos;
layout (location = 1) in vec3 a_vertNorm;

out vec3 v_normal;
out vec3 v_lightColor;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform vec3 u_lightColor;
uniform vec3 u_lightDir;


void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	
	v_normal = normalize((u_model * vec4(a_vertNorm, 0)).xyz);
	
	// Calculate lighting (half-lambartian).
	vec3 lightDir = normalize(u_lightDir);
	float lightAmount = dot(-lightDir, v_normal);
	lightAmount = max(0, lightAmount);
	v_lightColor = u_lightColor * lightAmount;
}
