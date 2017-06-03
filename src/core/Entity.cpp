#include "Entity.hpp"
#include "Transform.hpp"
#include "Engine.hpp"
#include "util/json_utils.hpp"
#include "util/type_registry.hpp"
#include "util/component_registry.hpp"
#include "scripting/Environment.hpp"
#include "scripting/class_registry.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Entity, "entity");

json_interpreter<Entity> Entity::s_properties({
	{ "name", &Entity::setName },
	{ "active", &Entity::setActive },
	{ "persistent", &Entity::setPersistent },
	{ "components", &Entity::extractComponents }
});

Entity::Entity() : m_id(Engine::instance()->getGUID()), m_active(true), m_persistent(false), m_transform(nullptr), m_parentScene(nullptr)
{
	scripting::Environment::instance()->createObject("Entity", this);

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

SCRIPTING_REGISTER_CLASS(Entity)

SCRIPTING_DEFINE_GETTER(Entity, getid, getId)
SCRIPTING_DEFINE_GETTER(Entity, getname, getName)
SCRIPTING_DEFINE_GETTER(Entity, isactive, isActive)
SCRIPTING_DEFINE_GETTER(Entity, ispersistent, isPersistent)

SCRIPTING_DEFINE_SETTER(Entity, setname, setName)
SCRIPTING_DEFINE_SETTER(Entity, setactive, setActive)
SCRIPTING_DEFINE_SETTER(Entity, setpersistent, setPersistent)

SCRIPTING_DEFINE_METHOD(Entity, getcomponent)
{
	// TODO
	return 0;
}

SCRIPTING_DEFINE_METHOD(Entity, addcomponent)
{
	// TODO
	return 0;
}
