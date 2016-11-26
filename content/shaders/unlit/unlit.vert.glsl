#version 330
#pragma type vertex

#include "common/input.glh"
#include "common/uniforms.glh"
#include "unlit_common.glh"

void main()
{
	gl_Position = cm_mat_wvp * vec4(v_input.position, 1.0);
}