#include "Component.hpp"
#include "Entity.hpp"
#include "scripting/Environment.hpp"
#include "scripting/class_registry.hpp"

json_interpreter<Component> Component::s_properties({
	{ "enabled", &Component::setEnabled }
});

Component::Component(Entity* parent) : m_parent(parent), m_enabled(true) { }
Component::~Component() { }

bool Component::isActiveAndEnabled() const
{
	return m_parent->isActive() && isEnabled();
}

void Component::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

SCRIPTING_REGISTER_CLASS(Component)

SCRIPTING_DEFINE_GETTER(Component, getentity, getEntity)
SCRIPTING_DEFINE_GETTER(Component, isenabled, isEnabled)
SCRIPTING_DEFINE_GETTER(Component, isactiveandenabled, isActiveAndEnabled)
SCRIPTING_DEFINE_SETTER(Component, setenabled, setEnabled)
