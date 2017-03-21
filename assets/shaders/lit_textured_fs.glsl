#version 330 core

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_lightColor;

out vec4 o_color;

uniform sampler2D s_texture;
uniform vec4 u_color;
uniform vec3 u_ambientLight;
uniform bool u_isLit;

void main()
{
	o_color = texture2D(s_texture, v_texCoord) * u_color;
	
	// Alpha test
    if (o_color.a <= 0.5)
        discard;

	if (u_isLit)
		o_color.rgb *= v_lightColor + u_ambientLight;
}
