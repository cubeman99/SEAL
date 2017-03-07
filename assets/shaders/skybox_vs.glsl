#version 330

layout (location = 0) in vec3 a_vertPosition;

out vec3 v_position;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(a_vertPosition, 1.0);
	v_position = normalize(a_vertPosition);
}

