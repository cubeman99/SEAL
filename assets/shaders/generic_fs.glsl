#version 330 core

//in vec2 v_texCoord;
in vec3 v_normal;
in vec4 v_color;
in vec3 v_lightColor;

out vec4 o_color;

//uniform sampler2D s_texture;


void main()
{
	o_color = v_color;
	o_color.rgb *= v_lightColor;
	//o_color = texture2D(s_texture, v_texCoord);
}

