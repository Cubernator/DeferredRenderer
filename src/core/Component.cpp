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

void destroy_component(Component* cmpt)
{
	cmpt->entity()->removeComponent(cmpt);
}

SCRIPTING_REGISTER_CLASS(Component)

SCRIPTING_AUTO_METHOD(Component, entity)
SCRIPTING_AUTO_METHOD(Component, isEnabled)
SCRIPTING_AUTO_METHOD(Component, isActiveAndEnabled)
SCRIPTING_AUTO_METHOD(Component, setEnabled)
