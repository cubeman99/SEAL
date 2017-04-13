#version 330 core

layout(location = 0) in vec3 a_vertPos;
layout(location = 1) in vec3 a_vertNorm;
layout(location = 3) in vec4 a_vertColor;

out vec3 v_normal;
out vec4 v_color;
out vec3 v_lightColor;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform vec3 u_lightColor = vec3(0.5, 0.5, 0.5);
uniform vec3 u_lightDir = vec3(-1, -1, -1);


void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	
	v_normal = normalize((u_model * vec4(a_vertNorm, 0)).xyz);
	v_color = a_vertColor;
	
	// Calculate lighting (half-lambartian).
	vec3 lightDir = normalize(u_lightDir);
	float lightAmount = dot(-lightDir, v_normal);
	lightAmount = max(0, lightAmount);
	v_lightColor = u_lightColor * lightAmount;
}
