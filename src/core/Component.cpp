#include "Component.hpp"
#include "Entity.hpp"
#include "scripting/Environment.hpp"

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

void Component::registerScriptClass()
{
	auto es = scripting::Environment::instance();

	es->addClass("Component");
	es->addMethods("Component", {
		{ "getentity", &Component::lua_getentity },
		{ "isenabled", &Component::lua_isenabled },
		{ "isactiveandenabled", &Component::lua_isactiveandenabled },
		{ "setenabled", &Component::lua_setenabled }
	});
}

int Component::lua_getentity(lua_State* L)
{
	auto self = scripting::check_self<Component>(L);
	scripting::push_value(L, self->getEntity());
	return 1;
}

int Component::lua_isenabled(lua_State* L)
{
	auto self = scripting::check_self<Component>(L);
	scripting::push_value(L, self->isEnabled());
	return 1;
}

int Component::lua_isactiveandenabled(lua_State* L)
{
	auto self = scripting::check_self<Component>(L);
	scripting::push_value(L, self->isActiveAndEnabled());
	return 1;
}

int Component::lua_setenabled(lua_State* L)
{
	auto self = scripting::check_self<Component>(L);
	self->setEnabled(scripting::check_arg<bool>(L, 2));
	return 0;
}
