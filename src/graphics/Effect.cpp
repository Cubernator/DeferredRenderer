#include "graphics/Effect.hpp"
#include "graphics/shader/ShaderProgram.hpp"
#include "graphics/Material.hpp"
#include "util/type_registry.hpp"
#include "core/Content.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Effect, "effect");

json_interpreter<Effect> Effect::s_properties({
	{ "renderQueue",	&Effect::extractRenderQueue },
	{ "renderType",		&Effect::extractRenderType },
	{ "properties",		&Effect::extractProperties },
	{ "passes",			&Effect::extractPasses }
});

json_interpreter<Effect::pass> Effect::pass::s_properties({
	{ "name",			&Effect::pass::getName },
	{ "lightMode",		&Effect::pass::getLightMode },
	{ "state",			&Effect::pass::getState },
	{ "program",		&Effect::pass::getProgram },
});

keyword_helper<Effect::render_queue> Effect::s_renderQueues({
	{ "background",		Effect::queue_background },
	{ "geometry",		Effect::queue_geometry },
	{ "alphaTest",		Effect::queue_alpha_test },
	{ "transparent",	Effect::queue_transparent },
	{ "effect",			Effect::queue_effect },
});

keyword_helper<Effect::render_type> Effect::s_renderTypes({
	{ "opaque",			Effect::type_opaque },
	{ "cutout",			Effect::type_cutout },
	{ "transparent",	Effect::type_transparent }
});

keyword_helper<Effect::light_mode> Effect::s_lightModes({
	{ "forwardBase",		Effect::light_forward_base },
	{ "forwardAdd",			Effect::light_forward_add },
	{ "deferred",			Effect::light_deferred },
	{ "shadowCast",			Effect::light_shadow_cast },
	{ "deferredAmbient",	Effect::light_deferred_ambient },
	{ "deferredLight",		Effect::light_deferred_light }
});


Effect::Effect() : m_renderType(type_opaque), m_queuePriority(queue_geometry) { }

Effect::pass::pass() : mode(light_forward_base), program(nullptr) { }


const Effect::pass* Effect::getPass(light_mode mode)
{
	auto& by_mode_index = m_passes.get<by_mode>();
	auto it = by_mode_index.find(mode);
	if (it != by_mode_index.end()) {
		return &*it;
	}
	return nullptr;
}

const Effect::pass* Effect::getPass(const std::string& name)
{
	auto& by_name_index = m_passes.get<by_name>();
	auto it = by_name_index.find(name);
	if (it != by_name_index.end()) {
		return &*it;
	}
	return nullptr;
}

void Effect::applyProperties(const pass* p, const Material* m)
{
	for (const shader_property& prop : m_properties) {
		const shader_property* matProp = m->getProperty(prop.id);
		// use default value if material doesn't contain this property
		if (!matProp)
			matProp = &prop;

		matProp->applyTo(p->program);
	}
}

void Effect::pass::apply_json_impl(const nlohmann::json& json)
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
		s_renderQueues.get(json.get<std::string>(), q);

	} else if (json.is_array() && json.size() >= 2) {
		auto& j0 = json.at(0);
		auto& j1 = json.at(1);
		if (j0.is_string())
			s_renderQueues.get(j0.get<std::string>(), q);
		if (j1.is_number_integer())
			offset = j1.get<int>();
	}

	m_queuePriority = int(q) + offset;
}

void Effect::extractRenderType(const nlohmann::json& json)
{
	s_renderTypes.get(json, m_renderType);
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
		pass newPass;
		newPass.apply_json(json);

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

void Effect::pass::getName(const nlohmann::json& json)
{
	if (json.is_string()) {
		name = json.get<std::string>();
	}
}

void Effect::pass::getLightMode(const nlohmann::json& json)
{
	s_lightModes.get(json, mode);
}

void Effect::pass::getState(const nlohmann::json& json)
{
	state.apply_json(json);
}

void Effect::pass::getProgram(const nlohmann::json& json)
{
	program = Content::instance()->getPooledFromJson<ShaderProgram>(json);
}
