#include "core/Component.hpp"
#include "component_registry.hpp"

#include <new>

using factory_map = component_registry::factory_map;

static std::aligned_storage_t<sizeof(factory_map), alignof(factory_map)> g_factories_buf;
factory_map& component_registry::s_factories = reinterpret_cast<factory_map&>(g_factories_buf);


void component_registry::registerFactory(factory_function fun, const std::string& name)
{
	s_factories.emplace(name, fun);
}

component_registry::component_pointer component_registry::get(const std::string& name, Entity* entity)
{
	auto it = s_factories.find(name);
	if (it != s_factories.end()) {
		auto fun = it->second;
		if (fun) {
			return fun(entity);
		}
	}
	return component_pointer();
}

static int schwarz_counter;

component_registry_initializer::component_registry_initializer()
{
	if (schwarz_counter++ == 0) new (&component_registry::s_factories) factory_map();
}

component_registry_initializer::~component_registry_initializer()
{
	if (--schwarz_counter == 0) (&component_registry::s_factories)->~factory_map();
}