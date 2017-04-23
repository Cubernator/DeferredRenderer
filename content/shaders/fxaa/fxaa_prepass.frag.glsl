#version 330
#pragma type fragment

#include "common/uniforms_img.glh"
#include "fxaa_common.glh"
#include "common/utils.glh"

layout(location = 0) out vec4 f_output;

void main()
{
    vec4 color = texture(img_source, v_output.uv);
    color.rgb = to_gamma(color.rgb); // convert color to gamma space
    color.a = luma(color.rgb); // compute luma in gamma space
    f_output = color;
}