#version 330
#pragma type fragment

#include "common/lighting.glh"
#include "pbr/pbr_brdf.glh"
#include "pbr_ambient_common.glh"

#define MODE_DIFFUSE 1
#define MODE_SPECULAR 2
#define MODE_SMOOTHNESS 3
#define MODE_NORMAL 4
#define MODE_DEPTH 5

uniform sampler2D gbuf_diffuse;
uniform sampler2D gbuf_specSmooth;
uniform sampler2D gbuf_normal;
uniform sampler2D gbuf_depth;

uniform int mode;
uniform float nearPlane;
uniform float farPlane;

in vertex_output v_output;

layout(location = 0) out vec4 f_output;

float linearize_depth(float d)
{
	return (2.0 * nearPlane) / (farPlane + nearPlane - d*(farPlane - nearPlane));
}

void main()
{
	vec3 color = vec3(1, 0, 1);

	if (mode == MODE_DIFFUSE) {
		color = texture(gbuf_diffuse, v_output.uv).rgb;
	} else if (mode == MODE_SPECULAR) {
		color = texture(gbuf_specSmooth, v_output.uv).rgb;
	} else if (mode == MODE_SMOOTHNESS) {
		color = texture(gbuf_specSmooth, v_output.uv).aaa;
	} else if (mode == MODE_NORMAL) {
		color = texture(gbuf_normal, v_output.uv).rgb;
	} else if (mode == MODE_DEPTH) {
		float d = linearize_depth(texture(gbuf_depth, v_output.uv).r);
		color = vec3(d);
	}

	f_output = vec4(color, 1.0);
}