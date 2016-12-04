#version 330
#pragma type fragment

#include "common/uniforms.glh"
#include "common/lighting.glh"
#include "pbr/pbr_brdf.glh"
#include "pbr/pbr_utils.glh"
#include "pbr_forward_common.glh"

uniform vec4 color;
uniform float smoothnessScale;
uniform float metallicScale;

uniform sampler2D mainTex;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;

in vertex_output v_output;

layout(location = 0) out vec4 f_output;

void main()
{
	vec3 n = normalize(v_output.normal);
	vec3 t = normalize(v_output.tangent);
	vec3 b = normalize(v_output.bitangent);
	vec3 v = normalize(v_output.viewVec);
	vec3 l = normalize(v_output.lightVec);
	
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

	vec3 diffCol, specCol;
	getDiffuseAndSpecular(albedo, metallic, diffCol, specCol);

	float roughness = getRealRoughness(smoothness);
	
	vec3 lightColor = light_color_auto(v_output.lightVec);
	vec3 color = pbs_brdf(diffCol, specCol, lightColor, roughness, ppNormal, v, l);

	// eliminate lighting artifacts caused by normal mapping
	//float ndotl = dot(n, l);
	//color *= (ndotl < 0.0) ? 0.0 : 1.0;

	color += pbs_brdf_ambient(diffCol, cm_light_ambient.rgb);

	f_output = vec4(color, 1.0);
}

