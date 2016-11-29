#version 330
#pragma type fragment

#include "common/uniforms.glh"
#include "common/gbuffer.glh"
#include "pbr/pbr_utils.glh"
#include "pbr_deferred_common.glh"

uniform vec4 color;
uniform float smoothnessScale;
uniform float metallicScale;

uniform sampler2D mainTex;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;

in vertex_output v_output;

void main()
{
	vec3 n = normalize(v_output.normal);
	vec3 t = normalize(v_output.tangent);
	vec3 b = normalize(v_output.bitangent);

	// normal mapping
	mat3 tanSpace = mat3(t, b, n);
	vec3 ppNormal = texture(normalMap, v_output.uv).rgb * 2.0 - 1.0;
	ppNormal = normalize(tanSpace * ppNormal);

	// sample surface color
	vec3 albedo = texture(mainTex, v_output.uv).rgb * color.rgb;

	// sample metallic and smoothness
	vec2 metallicSmoothness = texture(metallicMap, v_output.uv).rg;
	float metallic = metallicSmoothness.r * metallicScale;
	float smoothness = metallicSmoothness.g * smoothnessScale;

	getDiffuseAndSpecular(albedo, metallic, gbuf_diffuse.rgb, gbuf_specSmooth.rgb);

	gbuf_specSmooth.a = smoothness;
	gbuf_normal.rgb = ppNormal * 0.5 + 0.5;
}

