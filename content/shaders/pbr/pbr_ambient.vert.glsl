#version 330
#pragma type vertex

#include "pbr_light_common.glh"

uniform mat4 transform;

layout(location = 0) in vec3 inPos;

out vertex_output v_output;

void main()
{
	vec4 pos = transform * vec4(inPos, 1.0);
	v_output.uv = (pos.xy + 1.0) * 0.5;
	gl_Position = pos;
}