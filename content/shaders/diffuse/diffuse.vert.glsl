#version 330
#pragma type vertex

#include "../common/uniforms.glh"
#include "../common/lighting.glh"
#include "../common/input.glh"
#include "diffuse_common.glh"

out vertex_output v_output;

void main()
{
	vec4 pos = vec4(v_input.position, 1.0);
	vec4 normal = vec4(v_input.normal, 0.0);

	gl_Position = cm_mat_wvp * pos;

	vec4 wp = cm_mat_world * pos;
	v_output.lightVec = light_vec(wp.xyz);

	vec4 wn = cm_mat_world * normal;
	v_output.normal = wn.xyz;

	v_output.uv = v_input.uv;
}