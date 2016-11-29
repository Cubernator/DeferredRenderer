#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "util/json_utils.hpp"
#include "util/type_registry.hpp"
#include "util/component_registry.hpp"

#include "boost/uuid/random_generator.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Entity, "entity");

json_interpreter<Entity> Entity::s_properties({
	{ "name", &Entity::setName },
	{ "active", &Entity::setActive },
	{ "components", &Entity::extractComponents }
});

Entity::Entity() : m_id(boost::uuids::random_generator()()), m_active(true), m_transform(nullptr)
{
	m_transform = addComponent<Transform>();
}

void Entity::update()
{
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
