#version 330 core

in vec3 a_vertPos;
in vec3 a_vertNorm;
//in vec2 a_vertTexCoord;

//out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_lightColor;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform vec3 u_lightColor;
uniform vec3 u_lightDir;


void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	
	//v_texCoord = a_vertTexCoord;
	v_normal = normalize((u_model * vec4(a_vertNorm, 0)).xyz);
	
	// Calculate lighting (half-lambartian).
	vec3 lightDir = normalize(u_lightDir);
	float lightAmount = dot(-lightDir, v_normal);
	//lightAmount = (lightAmount + 1) / 2;
	//lightAmount *= lightAmount;
	lightAmount = max(0, lightAmount);
	v_lightColor = u_lightColor * lightAmount;
	//v_lightColor = (max(0, lightAmount) + vec3(1)) / 2; 
}
