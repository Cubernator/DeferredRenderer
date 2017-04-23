#version 330
#pragma type vertex

#include "common/vertex_input_img.glh"
#include "fxaa_common.glh"

void main()
{
	gl_Position = v_input.pos;
	v_output.uv = v_input.uv;
}
