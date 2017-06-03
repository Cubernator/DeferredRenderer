#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include <vector>
#include <string>

#include "Object.hpp"
#include "Component.hpp"
#include "util/import.hpp"
#include "util/json_interpreter.hpp"
#include "util/json_initializable.hpp"

#include "guid.hpp"

struct lua_State;

class Scene;
class Transform;

class Entity : public Object, public json_initializable<Entity>
{
public:
	Entity();
	virtual ~Entity();

	const guid& getId() const { return m_id; }

	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

	bool isActive() const { return m_active; }
	void setActive(bool val) { m_active = val; }

	bool isPersistent() const { return m_persistent; }
	void setPersistent(bool val) { m_persistent = val; }

	Scene* getParentScene() { return m_parentScene; }
	const Scene* getParentScene() const { return m_parentScene; }
	void setParentScene(Scene* scene) { m_parentScene = scene; }

	void start();
	void update();

	Transform* getTransform() { return m_transform; }
	const Transform* getTransform() const { return m_transform; }

	template<typename T>
	T* addComponent()
	{
		T* r = nullptr;
		if (!multiple_components_allowed<T>::type::value) {
			r = getComponent<T>();
		}

		if (!r) {
			m_components.push_back(std::make_unique<T>(this));
			r = static_cast<T*>(m_components.back().get());
		}

		return r;
	}

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
		return getTransform();
	}

	template<>
	const Transform* getComponent<Transform>() const
	{
		return getTransform();
	}

	template<typename T>
	std::vector<T*> getComponents()
	{
		return getComponentsInternal<T>();
	}

	template<typename T>
	std::vector<const T*> getComponents() const
	{
		return getComponentsInternal<T>();
	}

	template<typename T>
	bool hasComponent() const
	{
		return getComponent<T>();
	}

	static void registerScriptClass();

private:
	const guid m_id;
	std::string m_name;
	bool m_active;
	bool m_persistent;

	std::vector<std::unique_ptr<Component>> m_components;
	Transform* m_transform; // every entity must have a transform, so cache it here for fast access

	Scene* m_parentScene;

	static json_interpreter<Entity> s_properties;

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

	// cppcheck-suppress unusedPrivateFunction
	void apply_json_impl(const nlohmann::json& json);

	// cppcheck-suppress unusedPrivateFunction
	void extractComponents(const nlohmann::json& json);

	friend struct json_initializable<Entity>;

	static int lua_getid(lua_State* L);
	static int lua_getname(lua_State* L);
	static int lua_isactive(lua_State* L);
	static int lua_ispersistent(lua_State* L);

	static int lua_setname(lua_State* L);
	static int lua_setactive(lua_State* L);
	static int lua_setpersistent(lua_State* L);

	static int lua_getcomponent(lua_State* L);
	static int lua_addcomponent(lua_State* L);
};

#endif // ENTITY_HPP