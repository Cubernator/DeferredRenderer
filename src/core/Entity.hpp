#ifndef CORE_ENTITY_HPP
#define CORE_ENTITY_HPP

#include <memory>
#include <vector>
#include <string>
#include <typeindex>

#include "NamedObject.hpp"
#include "Component.hpp"
#include "util/import.hpp"
#include "util/json_interpreter.hpp"
#include "util/json_initializable.hpp"

namespace hexeract
{
	namespace scripting
	{
		class Behaviour;
	}

	class Scene;
	class Transform;

	class Entity : public NamedObject, public json_initializable<Entity>
	{
	public:
		Entity();
		~Entity();

		bool isActive() const { return m_active; }
		void setActive(bool val) { m_active = val; }

		bool isPersistent() const { return m_persistent; }
		void setPersistent(bool val) { m_persistent = val; }

		Scene* parentScene() { return m_parentScene; }
		const Scene* parentScene() const { return m_parentScene; }
		void setParentScene(Scene* scene) { m_parentScene = scene; }

		Transform* transform() { return m_transform; }
		const Transform* transform() const { return m_transform; }

		scripting::Behaviour* getBehaviour(const std::string& className)
		{
			return getBehaviourInternal(className);
		}
		const scripting::Behaviour* getBehaviour(const std::string& className) const
		{
			return getBehaviourInternal(className);
		}

		template<typename T>
		T* addComponent()
		{
			T* r = nullptr;
			if (!multiple_components_allowed<T>::value) {
				r = getComponent<T>();
			}

			if (!r) {
				m_components.push_back(std::make_unique<T>(this));
				r = static_cast<T*>(m_components.back().get());
				initComponent(typeid(T), r);
			}

			return r;
		}

		void removeComponent(Component* cmpt);

		template<typename T>
		T* getComponent()
		{
			return getComponentInternal<T>();
		}

		template<typename T>
		const T* getComponent() const
		{
			return getComponentInternal<T>();
		}

		template<>
		Transform* getComponent<Transform>()
		{
			return transform();
		}

		template<>
		const Transform* getComponent<Transform>() const
		{
			return transform();
		}

		template<typename T>
		std::vector<T*> getComponents()
		{
			return getComponentsInternal<T>();
		}

		template<typename T>
		std::vector<const T*> getComponents() const
		{
			return getComponentsInternal<const T>();
		}

		template<typename T>
		bool hasComponent() const
		{
			return getComponent<T>();
		}

		void preInit(const nlohmann::json& json);

	private:
		bool m_active, m_persistent;

		using cmpt_ptr = std::unique_ptr<Component>;

		std::vector<cmpt_ptr> m_components;
		std::vector<scripting::Behaviour*> m_behaviours;
		// every entity must have a transform, so cache it here for fast access
		Transform* m_transform;

		Scene* m_parentScene;

		static json_interpreter<Entity> s_properties;

		void initComponent(std::type_index id, Component* cmpt);


		template<typename T>
		T* getComponentInternal() const
		{
			static_assert(std::is_base_of<Component, T>::value, "Only subclasses of Component are allowed!");

			for (auto& component : m_components) {
				T* c = dynamic_cast<T*>(component.get());
				if (c) return c;
			}

			return nullptr;
		}

		template<typename T>
		std::vector<T*> getComponentsInternal() const
		{
			static_assert(std::is_base_of<Component, T>::value, "Only subclasses of Component are allowed!");

			std::vector<T*> result;

			for (auto& component : m_components) {
				T* c = dynamic_cast<T*>(component.get());
				if (c) result.push_back(c);
			}

			return result;
		}

		scripting::Behaviour* getBehaviourInternal(const std::string& className) const;

		// cppcheck-suppress unusedPrivateFunction
		void apply_json_impl(const nlohmann::json& json);

		// cppcheck-suppress unusedPrivateFunction
		void extractComponents(const nlohmann::json& json);

		friend struct json_initializable<Entity>;
	};

	Entity* create_entity(const std::string& name);

	inline Entity* create_entity()
	{
		return create_entity("unnamed");
	}
}

#endif // CORE_ENTITY_HPP