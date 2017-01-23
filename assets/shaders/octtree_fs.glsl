#version 330 core

in vec3 v_vertPos;

out vec4 o_color;

uniform vec4 u_color;
uniform float u_worldRadius = 0.8;

void main()
{
	float r = length(v_vertPos) - u_worldRadius;
	
	if (r > 0 && r <= u_worldRadius * 0.005)
		o_color = u_color;
	else
		discard;
}
