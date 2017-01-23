#version 330 core

in vec3 a_vertPos;

out vec3 v_vertPos;

uniform mat4 u_mvp;
uniform mat4 u_model;

void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	v_vertPos = (u_model * vec4(a_vertPos, 1.0)).xyz;
}
