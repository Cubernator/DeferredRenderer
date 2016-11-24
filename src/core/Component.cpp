#include "core/Component.hpp"
#include "core/Entity.hpp"

json_interpreter<Component> Component::s_properties({
	{ "enabled", &Component::extractEnabled }
});

Component::Component(Entity* parent) : m_parent(parent) { }
Component::~Component() { }

bool Component::isActiveAndEnabled() const
{
	return m_parent->isActive() && isEnabled();
}

void Component::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	s_properties.interpret_property(name, this, json);
}

void Component::extractEnabled(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		setEnabled(json);
	}
}
