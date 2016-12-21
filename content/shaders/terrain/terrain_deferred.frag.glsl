#version 330
#pragma type fragment

#define WORKFLOW_SPECULAR

#include "terrain_input.glh"
#include "common/vertex_output.glh"
#include "pbr/pbr_gbuf.glh"

void main()
{
	pbr_data data = terrain_get_data(v_output.uv0, v_output.tangentSpace);
	vec3 diff, spec;
	pbr_get_diff_spec(data, diff, spec);
	pbr_set_gbuffer(diff, spec, data.smoothness, data.normal);
}