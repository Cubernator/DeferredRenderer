#version 330
#pragma type fragment

#include "../common/uniforms.glh"
#include "unlit_common.glh"

uniform vec4 color;

layout(location = 0) out vec4 finalColor;

void main()
{
	finalColor = color;
}

