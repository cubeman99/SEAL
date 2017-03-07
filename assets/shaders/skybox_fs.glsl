#version 330

in vec3 v_position;

out vec4 o_color;

uniform samplerCube u_texture;
//uniform sampler2D u_texture;

void main()
{
	o_color = texture(u_texture, normalize(v_position));
	//o_color = texture(u_texture, (v_position.xy + vec2(1)) / 2);
	//o_color = vec4(1,0,0,1);
	//o_color = vec4((v_position + vec3(1)) / 2, 1);
}
