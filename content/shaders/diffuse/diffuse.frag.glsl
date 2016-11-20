#version 330
#pragma type fragment

#include "../common/uniforms.glh"
#include "../common/lighting.glh"
#include "diffuse_common.glh"

uniform vec4 color;
uniform sampler2D mainTex;

in vertex_output v_output;

layout(location = 0) out vec4 f_output;

void main()
{
	vec3 n = normalize(v_output.normal);

	vec3 diffCol = texture(mainTex, v_output.uv).rgb * color.rgb;
	vec3 diffLight = light_lambert(n, v_output.lightVec) * cm_light_color.rgb;

	vec3 finalColor = diffCol * diffLight;
	f_output = vec4(finalColor, 1.0);
}

