#version 330
#pragma type fragment

#include "common/lighting.glh"
#include "common/utils.glh"
#include "pbr_light_common.glh"

uniform sampler2D gbuf_diffuse;
uniform sampler2D gbuf_depth;

in vertex_output v_output;

layout(location = 0) out vec4 f_output;

void main()
{
	// read from G-Buffer
	vec3 diffCol = texture(gbuf_diffuse, v_output.uv).rgb;
	float depth = texture(gbuf_depth, v_output.uv).r;

	// apply ambient light
	vec3 color = (diffCol * cm_light_ambient.rgb) / PI;

	f_output = vec4(color, 1.0);
	gl_FragDepth = depth;
}