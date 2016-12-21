#version 330
#pragma type fragment

#include "common/lighting.glh"
#include "pbr/pbr_brdf.glh"
#include "pbr_light_common.glh"

uniform sampler2D gbuf_diffuse;
uniform sampler2D gbuf_specSmooth;
uniform sampler2D gbuf_normal;
uniform sampler2D gbuf_depth;

uniform mat4 cm_mat_ivp;
uniform vec3 cm_cam_pos;

in vertex_output v_output;

layout(location = 0) out vec4 f_output;

void main()
{
	// read from G-Buffer
	vec3 diffCol = texture(gbuf_diffuse, v_output.uv).rgb;
	vec4 specSmooth = texture(gbuf_specSmooth, v_output.uv);
	vec3 normal = texture(gbuf_normal, v_output.uv).rgb * 2.0 - 1.0;
	float depth = texture(gbuf_depth, v_output.uv).r;

	// calculate fragment world position with inverse view+projection matrix
	vec4 wp = cm_mat_ivp * vec4(v_output.pos, depth * 2.0 - 1.0, 1.0);
	vec3 worldPos = wp.xyz / wp.w;

	// get light and view vectors
	vec3 lightVec = light_vec(worldPos);
	vec3 l = normalize(lightVec);
	vec3 v = normalize(cm_cam_pos - worldPos);

	vec3 lightColor = light_color_auto(lightVec);
	float roughness = get_roughness(specSmooth.a);

	// apply BRDF
	vec3 color = pbr_brdf(diffCol, specSmooth.rgb, lightColor, roughness, normal, v, l);

	f_output = vec4(color, 1.0);
}