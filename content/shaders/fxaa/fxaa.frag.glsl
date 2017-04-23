#version 420
#pragma type fragment

#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_QUALITY__PRESET 29

#include "fxaa3_11.glh"

#include "common/uniforms_img.glh"
#include "common/utils.glh"
#include "fxaa_common.glh"

uniform float subpix;
uniform float edgeThresholdMin;
uniform float edgeThreshold;

layout(location = 0) out vec4 f_output;

void main()
{
    f_output = FxaaPixelShader(
        v_output.uv,
        vec4(0.0),
        img_source,
        img_source,
        img_source,
        img_resolution.zw,
        vec4(0.0),
        vec4(0.0),
        vec4(0.0),
        subpix,
        edgeThreshold,
        edgeThresholdMin,
        0.0,
        0.0,
        0.0,
        vec4(0.0)
    );

    /*
    HACK: convert output back to linear space and enable GL_FRAMEBUFFER_SRGB
    to take advantage of OpenGL's builtin conversion.
    This is done to make sure the output is consistent with the original image.
    */
    f_output.rgb = to_linear(f_output.rgb);
}