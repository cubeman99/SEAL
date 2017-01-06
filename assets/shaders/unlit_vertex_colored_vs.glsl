#version 330 core

in vec3 a_vertPos;
in vec4 a_vertColor;

out vec4 v_color;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(a_vertPos, 1.0);
	v_color = a_vertColor;
}
