#version 330 core

//in vec2 v_texCoord;
in vec3 v_normal;
in vec4 v_color;
in vec3 v_lightColor;

out vec4 o_color;

//uniform sampler2D s_texture;
uniform vec4 u_color =  vec4(1,1,1,1);
uniform vec3 u_ambientLight = vec3(0.5, 0.5, 0.5);
uniform bool u_isLit;

void main()
{
	//o_color = u_color;
	o_color = v_color * u_color;
	//o_color = texture2D(s_texture, v_texCoord) * u_color;
	if (u_isLit)
		o_color.rgb *= v_lightColor + u_ambientLight;
}
