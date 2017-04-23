#include "Component.hpp"
#include "Entity.hpp"

json_interpreter<Component> Component::s_properties({
	{ "enabled", &Component::setEnabled }
});

Component::Component(Entity* parent) : m_parent(parent), m_enabled(true) { }
Component::~Component() { }

bool Component::isActiveAndEnabled() const
{
	return m_parent->isActive() && isEnabled();
}

void Component::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	s_properties.interpret_property(name, this, json);
}
