#include "LightSwitch.hpp"
#include "input/Input.hpp"
#include "core/Entity.hpp"
#include "graphics/Light.hpp"
#include "util/component_registry.hpp"

REGISTER_COMPONENT_CLASS(LightSwitch, "lightSwitch");

json_interpreter<LightSwitch> LightSwitch::s_properties{
	{ "key", &LightSwitch::m_key }
};

LightSwitch::LightSwitch(Entity* parent) : Component(parent), m_light(nullptr), m_key(key_unknown) { }

void LightSwitch::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void LightSwitch::start_impl()
{
	m_light = getEntity()->getComponent<Light>();
}

void LightSwitch::update_impl()
{
	if (m_light && Input::instance()->getKeyPressed(m_key)) {
		m_light->setEnabled(!m_light->isEnabled());
	}
}
