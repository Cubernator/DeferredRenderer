#version 330
#pragma type fragment

#include "../common/uniforms.glh"
#include "../common/lighting.glh"
#include "pbr_common.glh"
#include "pbr_brdf.glh"

uniform vec4 color;
uniform float smoothness;
uniform float metallic;
uniform float dielectricReflec;

uniform sampler2D mainTex;
uniform sampler2D normalMap;

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

	// calculate diffuse and specular colors depending on metallic factor and according to energy conservation
	float omd = 1.0 - dielectricReflec;
	vec3 diffCol = albedo * (omd - metallic * omd);

	// The BRDF treats the specular color as the component-wise reflectance at 0°
	// For non-metallic (dielectric) materials the reflectance is defined by the "dielectricReflec" uniform (in most cases constant at 0.22).
	// For metals the specular color is "tinted" by the surface color (albedo).
	// These values are blended together depending of the metallic value.
	vec3 specCol = mix(vec3(dielectricReflec), albedo, metallic);

	// smoothness is 1.0 - sqrt(m), where m is the real academic roughness (which is needed in BRDF)
	float roughness = 1.0 - smoothness;
	roughness *= roughness;

	vec3 lightColor = light_attenuated_color(v_output.lightVec);
	vec3 color = pbs_brdf(diffCol, specCol, lightColor, roughness, ppNormal, v, l);

	// eliminate lighting artifacts caused by normal mapping
	float ndotl = dot(n, l);
	color *= (ndotl < 0.0) ? 0.0 : 1.0;

	f_output = vec4(color, 1.0);
}

