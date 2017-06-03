#include "Entity.hpp"
#include "Transform.hpp"
#include "Engine.hpp"
#include "util/json_utils.hpp"
#include "util/type_registry.hpp"
#include "util/component_registry.hpp"
#include "scripting/Environment.hpp"
#include "lua.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Entity, "entity");

json_interpreter<Entity> Entity::s_properties({
	{ "name", &Entity::setName },
	{ "active", &Entity::setActive },
	{ "persistent", &Entity::setPersistent },
	{ "components", &Entity::extractComponents }
});

Entity::Entity() : m_id(Engine::instance()->getGUID()), m_active(true), m_persistent(false), m_transform(nullptr), m_parentScene(nullptr)
{
	auto m_scriptEnv = scripting::Environment::instance();

	m_scriptEnv->createObject("Entity", this);

	m_transform = addComponent<Transform>();
}

Entity::~Entity()
{
	scripting::Environment::instance()->invalidateObject(this);
}

void Entity::start()
{
	if (!m_active) return;

	for (auto& c : m_components) {
		c->start();
	}
}

void Entity::update()
{
	if (!m_active) return;

	for (auto& c : m_components) {
		c->update();
	}
}

void Entity::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Entity::extractComponents(const nlohmann::json& json)
{
	if (json.is_array()) {
		for (auto cj : json) {
			Component* c = component_registry::get(get_type(cj), this);
			if (c)
				c->apply_json(cj);
		}
	}
}

void Entity::registerScriptClass()
{
	auto se = scripting::Environment::instance();
	se->addClass("Entity");

	se->addMethods("Entity", {
		{ "getid",			&Entity::lua_getid },
		{ "getname",		&Entity::lua_getname },
		{ "isactive",		&Entity::lua_isactive },
		{ "ispersistent",	&Entity::lua_ispersistent },
		{ "setname",		&Entity::lua_setname },
		{ "setactive",		&Entity::lua_setactive },
		{ "setpersistent",	&Entity::lua_setpersistent },
		{ "getcomponent",	&Entity::lua_getcomponent },
		{ "addcomponent",	&Entity::lua_addcomponent }
	});
}

int Entity::lua_getid(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	scripting::push_value(L, self->getId());
	return 1;
}

int Entity::lua_getname(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	scripting::push_value(L, self->getName());
	return 1;
}

int Entity::lua_isactive(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	scripting::push_value(L, self->isActive());
	return 1;
}

int Entity::lua_ispersistent(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	scripting::push_value(L, self->isPersistent());
	return 1;
}

int Entity::lua_setname(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	self->setName(scripting::check_arg<std::string>(L, 2));
	return 0;
}

int Entity::lua_setactive(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	self->setActive(scripting::check_arg<bool>(L, 2));
	return 0;
}

int Entity::lua_setpersistent(lua_State* L)
{
	auto self = scripting::check_self<Entity>(L);
	self->setPersistent(scripting::check_arg<bool>(L, 2));
	return 0;
}

int Entity::lua_getcomponent(lua_State* L)
{
	// TODO
	return 0;
}

int Entity::lua_addcomponent(lua_State* L)
{
	// TODO
	return 0;
}
