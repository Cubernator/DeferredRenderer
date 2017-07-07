#ifndef COMPONENT_UTILS_HPP
#define COMPONENT_UTILS_HPP

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"

namespace mi = boost::multi_index;

class Entity;
class Component;

struct component_factory
{
	virtual Component* getFirst(Entity* entity) const = 0;
	virtual const Component* getFirst(const Entity* entity) const = 0;
	virtual std::vector<Component*> getAll(Entity* entity) const = 0;
	virtual std::vector<const Component*> getAll(const Entity* entity) const = 0;
	virtual Component* add(Entity* entity) const = 0;
};

struct component_type
{
	std::type_index id;
	std::string name;
	std::shared_ptr<component_factory> factory;

	component_type();
	component_type(std::type_index id, const std::string& name, std::shared_ptr<component_factory> factory);

	operator bool() const;
};

class component_registry
{
public:
	static void registerComponentType(component_type type);

	static const component_type& findByName(const std::string& name);
	static const component_type& findById(std::type_index id);

	struct by_name { };
	struct by_id { };

	struct type_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_name>, mi::member<component_type, std::string, &component_type::name>>,
		mi::hashed_unique<mi::tag<by_id>, mi::member<component_type, std::type_index, &component_type::id>>
	> { };

	using type_container = mi::multi_index_container<
		component_type,
		type_container_indices
	>;

private:
	static type_container& s_types;
	static component_type s_empty_type;

	friend struct component_registry_initializer;
};

template<typename T>
using multiple_components_allowed = typename T::multiple_allowed;


namespace detail
{
	template<typename T>
	struct abstract_cmpt_factory : public component_factory
	{
		virtual Component* getFirst(Entity* entity) const final
		{
			return entity->getComponent<T>();
		}
		virtual const Component* getFirst(const Entity* entity) const final
		{
			return entity->getComponent<T>();
		}
		virtual std::vector<Component*> getAll(Entity* entity) const final
		{
			auto cmpts = entity->getComponents<T>();
			return std::vector<Component*>(cmpts.begin(), cmpts.end());
		}
		virtual std::vector<const Component*> getAll(const Entity* entity) const final
		{
			auto cmpts = entity->getComponents<T>();
			return std::vector<const Component*>(cmpts.begin(), cmpts.end());
		}
		virtual Component* add(Entity* entity) const override
		{
			throw std::runtime_error("cannot add abstract component type to entity!");
		}
	};

	template<typename T>
	struct component_factory_t : public abstract_cmpt_factory<T>
	{
		virtual Component* add(Entity* entity) const final
		{
			return entity->addComponent<T>();
		}
	};

	template<typename T>
	struct abstract_component_registerer
	{
		explicit abstract_component_registerer(const std::string& name)
		{
			component_registry::registerComponentType(component_type(typeid(T), name, std::make_shared<abstract_cmpt_factory<T>>()));
		}
	};

	template<typename T>
	struct component_registerer
	{
		explicit component_registerer(const std::string& name)
		{
			component_registry::registerComponentType(component_type(typeid(T), name, std::make_shared<component_factory_t<T>>()));
		}
	};
}

#define REGISTER_ABSTRACT_COMPONENT_CLASS(c)								\
namespace																	\
{																			\
	::detail::abstract_component_registerer<c> register_component_##c (#c);	\
}

#define REGISTER_COMPONENT_CLASS(c)								\
namespace														\
{																\
	::detail::component_registerer<c> register_component_##c (#c);\
}


static struct component_registry_initializer
{
	component_registry_initializer();
	~component_registry_initializer();
} component_registry_init;

#endif // COMPONENT_UTILS_HPP