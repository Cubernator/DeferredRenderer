#version 330
#pragma type fragment

#define PATH_FORWARD
#define WORKFLOW_SPECULAR

#include "pbr/pbr_brdf.glh"
#include "terrain_input.glh"
#include "common/vertex_output.glh"
#include "common/lighting.glh"

layout(location = 0) out vec4 f_output;

void main()
{
	vec3 v = normalize(v_output.viewVec);
	vec3 l = normalize(v_output.lightVec);

	pbr_data data = terrain_get_data(v_output.uv0, v_output.tangentSpace);
	vec3 diff, spec;
	pbr_get_diff_spec(data, diff, spec);

	vec3 lightColor = light_color_auto(v_output.lightVec);
	vec3 color = pbr_brdf(diff, spec, lightColor, get_roughness(data.smoothness), data.normal, v, l);

	color += pbr_brdf_ambient(diff, cm_light_ambient.rgb);

	f_output = vec4(color, 1.0);
}