#ifndef COMPONENT_UTILS_HPP
#define COMPONENT_UTILS_HPP

#include <unordered_map>
#include <functional>


class Entity;
class Component;

class component_registry
{
public:
	using component_pointer = Component*;
	using factory_function = std::function<component_pointer(Entity*)>;
	using factory_map = std::unordered_map<std::string, factory_function>;

	static void registerFactory(factory_function fun, const std::string& name);
	static component_pointer get(const std::string& name, Entity* entity);

private:
	static factory_map& s_factories;

	friend struct component_registry_initializer;
};


template<typename T>
struct multiple_components_allowed
{
	using type = typename T::multiple_allowed;
};


template<typename T>
struct component_adder
{
	T* operator() (Entity* entity) const
	{
		return entity->addComponent<T>();
	}
};


template<typename T>
struct component_registerer
{
	component_registerer(const std::string& name)
	{
		component_registry::registerFactory(component_adder<T>(), name);
	}
};

#define REGISTER_COMPONENT_CLASS_DECL(c) static component_registerer<c> s_cmp_register_helper
#define REGISTER_COMPONENT_CLASS_DEF(c, n) component_registerer<c> c::s_cmp_register_helper((n))


static struct component_registry_initializer
{
	component_registry_initializer();
	~component_registry_initializer();
} component_registry_init;

#endif // COMPONENT_UTILS_HPP