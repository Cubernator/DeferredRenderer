#version 330
#pragma type fragment

#include "common/uniforms.glh"
#include "common/gbuffer.glh"
#include "pbr_deferred_common.glh"
#include "pbr_utils.glh"

uniform vec4 color;
uniform float smoothness;
uniform float metallic;

uniform sampler2D mainTex;
uniform sampler2D normalMap;

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

	getDiffuseAndSpecular(albedo, metallic, gbuf_diffuse.rgb, gbuf_specSmooth.rgb);

	gbuf_specSmooth.a = smoothness;
	gbuf_normal.rgb = ppNormal * 0.5 + 0.5;
}

