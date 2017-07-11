#include "core/Component.hpp"
#include "component_registry.hpp"

#include <new>

using type_container = component_registry::type_container;

component_type component_registry::s_empty_type;

struct null_component_factory : public component_factory
{
	virtual Component* getFirst(Entity* entity) const { return nullptr; }
	virtual const Component* getFirst(const Entity* entity) const { return nullptr; }
	virtual std::vector<Component*> getAll(Entity* entity) const { return std::vector<Component*>(); }
	virtual std::vector<const Component*> getAll(const Entity* entity) const { return std::vector<const Component*>(); }
	virtual Component* add(Entity* entity) const { return nullptr; }
};

component_type::component_type() : id(typeid(void)), name("unknown"), factory(std::make_shared<null_component_factory>()) { }
component_type::component_type(std::type_index id, const std::string& name, std::shared_ptr<component_factory> factory)
	: id(id), name(name), factory(factory) { }

component_type::operator bool() const
{
	return id != typeid(void);
}


void component_registry::registerComponentType(component_type type)
{
	s_types.insert(type);
}

const component_type& component_registry::findByName(const std::string& name)
{
	auto& idx = s_types.get<by_name>();
	auto it = idx.find(name);
	if (it != idx.end()) {
		return *it;
	}
	return s_empty_type;
}

const component_type& component_registry::findById(std::type_index id)
{
	auto& idx = s_types.get<by_id>();
	auto it = idx.find(id);
	if (it != idx.end()) {
		return *it;
	}
	return s_empty_type;
}


static std::aligned_storage_t<sizeof(type_container), alignof(type_container)> g_factories_buf;
type_container& component_registry::s_types = reinterpret_cast<type_container&>(g_factories_buf);

static int schwarz_counter;

component_registry_initializer::component_registry_initializer()
{
	if (schwarz_counter++ == 0) new (&component_registry::s_types) type_container();
}

component_registry_initializer::~component_registry_initializer()
{
	if (--schwarz_counter == 0) (&component_registry::s_types)->~type_container();
}
