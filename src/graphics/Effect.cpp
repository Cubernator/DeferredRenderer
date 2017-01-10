#include "Effect.hpp"
#include "shader/ShaderProgram.hpp"
#include "Material.hpp"
#include "util/type_registry.hpp"
#include "core/Content.hpp"

namespace
{
	keyword_helper<render_queue> g_renderQueues({
		{ "background",		queue_background },
		{ "geometry",		queue_geometry },
		{ "alphaTest",		queue_alpha_test },
		{ "transparent",	queue_transparent },
		{ "effect",			queue_effect },
	});

	keyword_helper<render_type> g_renderTypes({
		{ "opaque",			type_opaque },
		{ "cutout",			type_cutout },
		{ "transparent",	type_transparent }
	});

	keyword_helper<light_mode> g_lightModes({
		{ "forwardBase",		light_forward_base },
		{ "forwardAdd",			light_forward_add },
		{ "deferred",			light_deferred },
		{ "shadowCast",			light_shadow_cast }
	});
}

REGISTER_OBJECT_TYPE_NO_EXT(Effect, "effect");

json_interpreter<Effect> Effect::s_properties({
	{ "renderQueue",	&Effect::extractRenderQueue },
	{ "renderType",		{&Effect::setRenderType, &g_renderTypes} },
	{ "properties",		&Effect::extractProperties },
	{ "passes",			&Effect::extractPasses }
});

json_interpreter<Pass> Pass::s_properties({
	{ "name",			&Pass::name },
	{ "lightMode",		{&Pass::mode, &g_lightModes} },
	{ "state",			&Pass::extractState },
	{ "program",		&Pass::extractProgram },
});


Effect::Effect() : m_renderType(type_opaque), m_queuePriority(queue_geometry) { }

Pass::Pass() : mode(light_forward_base), program(nullptr) { }


const Pass* Effect::getPass(light_mode mode) const
{
	auto& by_mode_index = m_passes.get<by_mode>();
	auto it = by_mode_index.find(mode);
	if (it != by_mode_index.end()) {
		return &*it;
	}
	return nullptr;
}

const Pass* Effect::getPass(const std::string& name) const
{
	auto& by_name_index = m_passes.get<by_name>();
	auto it = by_name_index.find(name);
	if (it != by_name_index.end()) {
		return &*it;
	}
	return nullptr;
}

void Effect::applyProperties(const ShaderProgram* p, const Material* m) const
{
	for (const shader_property& prop : m_properties) {
		const shader_property* matProp = m->getProperty(prop.id);
		// use default value if material doesn't contain this property
		if (!matProp)
			matProp = &prop;

		matProp->applyTo(p);
	}
}

void Pass::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Effect::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Effect::extractRenderQueue(const nlohmann::json& json)
{
	render_queue q = queue_geometry;
	int offset = 0;

	if (json.is_string()) {
		g_renderQueues.get(json.get<std::string>(), q);

	} else if (json.is_array() && json.size() >= 2) {
		auto& j0 = json.at(0);
		auto& j1 = json.at(1);
		if (j0.is_string())
			g_renderQueues.get(j0.get<std::string>(), q);
		if (j1.is_number_integer())
			offset = j1.get<int>();
	}

	m_queuePriority = int(q) + offset;
}

void Effect::extractProperties(const nlohmann::json& json)
{
	if (json.is_array()) {
		for (auto& pj : json) addProperty(pj);
	}
}

void Effect::extractPasses(const nlohmann::json& json)
{
	if (json.is_array()) {
		for (auto& pj : json) addPass(pj);
	}
}

void Effect::addProperty(const nlohmann::json& json)
{
	if (json.is_object()) {
		shader_property prop;
		prop.apply_json(json);
		m_properties.insert(prop);
	}
}

void Effect::addPass(const nlohmann::json& json)
{
	if (json.is_object()) {
		Pass newPass;
		newPass.apply_json(json);

		if (newPass.program) {
			if (!newPass.program->isGood()) {
				std::cout << "shader program has errors (" << newPass.name << "):" << std::endl;
				std::cout << newPass.program->getLog() << std::endl;
			}

			auto p = m_passes.insert(std::move(newPass));
			if (!p.second) {
				// TODO: print warning
			}
		}
	}
}

void Pass::extractState(const nlohmann::json& json)
{
	state.apply_json(json);
}

void Pass::extractProgram(const nlohmann::json& json)
{
	program = Content::instance()->getPooledFromJson<ShaderProgram>(json);
}
