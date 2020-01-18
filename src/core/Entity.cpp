#include "Entity.hpp"
#include "Transform.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "ObjectRegistry.hpp"
#include "type_registry.hpp"
#include "component_registry.hpp"
#include "util/json_utils.hpp"
#include "scripting/Environment.hpp"
#include "scripting/Behaviour.hpp"
#include "scripting/class_registry.hpp"

namespace hexeract
{
	REGISTER_OBJECT_TYPE_NO_EXT(Entity);

	namespace
	{
		json_interpreter<Entity> g_properties({
			{ "active", &Entity::setActive },
			{ "persistent", &Entity::setPersistent }
		});
	}

	json_interpreter<Entity> Entity::s_properties({
		{ "active", &Entity::setActive },
		{ "persistent", &Entity::setPersistent },
		{ "components", &Entity::extractComponents }
	});

	Entity::Entity() : m_active(true), m_persistent(false), m_transform(nullptr), m_parentScene(nullptr)
	{
		m_transform = addComponent<Transform>();
	}

	Entity::~Entity()
	{
		while (m_components.size() > 0) {
			removeComponent(m_components.back().get());
		}
	}

	void Entity::initComponent(std::type_index id, Component* cmpt)
	{
		if (id != typeid(scripting::Behaviour)) {
			const component_type& type = component_registry::findById(id);
			instance<scripting::Environment>()->createObject(type.name, cmpt);
		} else {
			m_behaviours.push_back(static_cast<scripting::Behaviour*>(cmpt));
		}

		instance<Engine>()->addComponent(cmpt);
	}

	void Entity::removeComponent(Component* cmpt)
	{
		instance<Engine>()->removeComponent(cmpt);

		if (typeid(*cmpt) != typeid(scripting::Behaviour)) {
			scripting::Environment::instance()->invalidateObject(cmpt);
		}

		m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [cmpt](const cmpt_ptr& c) {
			return cmpt == c.get();
		}), m_components.end());
	}

	void Entity::apply_json_impl(const nlohmann::json& json)
	{
		s_properties.interpret_all(this, json);
	}

	void Entity::preInit(const nlohmann::json& json)
	{
		g_properties.interpret_all(this, json);
	}

	void Entity::extractComponents(const nlohmann::json& json)
	{
		if (json.is_array()) {
			for (auto cj : json) {
				auto type = component_registry::findByName(get_type(cj));
				if (type) {
					auto cmpt = type.factory->add(this);
					if (cmpt) {
						cmpt->apply_json(cj);
					}
				}
			}
		}
	}

	scripting::Behaviour* Entity::getBehaviourInternal(const std::string& className) const
	{
		for (auto b : m_behaviours) {
			// TODO: make polymorphism work
			if (b->script() == className) {
				return b;
			}
		}
		return nullptr;
	}

	Entity* create_entity(const std::string& name)
	{
		Entity* e = instance<ObjectRegistry>()->emplace<Entity>(name);
		Scene* cs = instance<Engine>()->scene();
		if (cs) {
			cs->addEntity(e);
		}
		return e;
	}


	const component_type& check_cmpt_type(lua_State* L, int arg)
	{
		auto n = scripting::check_arg<std::string>(L, arg);
		const component_type& type = component_registry::findByName(n);
		if (!type) {
			luaL_argerror(L, arg, ("unknown component type: " + n).c_str());
		}
		return type;
	}

	SCRIPTING_REGISTER_DERIVED_CLASS(Entity, NamedObject);

		SCRIPTING_AUTO_METHOD(Entity, isActive)
		SCRIPTING_AUTO_METHOD(Entity, isPersistent)
		SCRIPTING_AUTO_METHOD(Entity, transform)
		SCRIPTING_AUTO_METHOD(Entity, parentScene)

		SCRIPTING_AUTO_METHOD(Entity, setActive)
		SCRIPTING_AUTO_METHOD(Entity, setPersistent)

		SCRIPTING_DEFINE_METHOD(Entity, getComponent)
	{
		auto self = scripting::check_self<Entity>(L);
		scripting::push_object(L, check_cmpt_type(L, 2).factory->getFirst(self));
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, getComponents)
	{
		auto self = scripting::check_self<Entity>(L);
		scripting::push_value(L, check_cmpt_type(L, 2).factory->getAll(self));
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, addComponent)
	{
		auto self = scripting::check_self<Entity>(L);
		scripting::push_object(L, check_cmpt_type(L, 2).factory->add(self));
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, getBehaviour)
	{
		auto self = scripting::check_self<Entity>(L);
		auto cn = scripting::check_arg<std::string>(L, 2);
		scripting::push_object(L, self->getBehaviour(cn));
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, addBehaviour)
	{
		auto self = scripting::check_self<Entity>(L);
		auto behaviour = self->addComponent<scripting::Behaviour>();
		behaviour->setScript(scripting::check_arg<std::string>(L, 2));
		scripting::push_object(L, behaviour);
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, create)
	{
		auto newEntity = create_entity(scripting::check_arg<std::string>(L, 1));
		scripting::push_object(L, newEntity);
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, findFirstByName)
	{
		scripting::push_object(L,
			instance<ObjectRegistry>()->findTNFirst<Entity>(
				scripting::check_arg<std::string>(L, 1)
				)
		);
		return 1;
	}

	SCRIPTING_DEFINE_METHOD(Entity, findAllByName)
	{
		scripting::push_value(L,
			instance<ObjectRegistry>()->findTNAll<Entity>(
				scripting::check_arg<std::string>(L, 1)
				)
		);
		return 1;
	}
}
