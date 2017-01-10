#ifndef TYPE_REGISTRY_HPP
#define TYPE_REGISTRY_HPP

#include <typeindex>
#include <string>

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"

namespace mi = boost::multi_index;

struct registered_type
{
	std::type_index type;
	std::string name;
	std::string extension;

	registered_type();
	registered_type(std::type_index type, const std::string& name, const std::string& extension);

	operator bool() const;
};

class type_registry
{
public:
	static void registerType(registered_type type);

	static const registered_type& findByName(const std::string& name);
	static const registered_type& findByType(std::type_index type);
	static const registered_type& findByExtension(const std::string& ext);

	struct by_type { };
	struct by_name { };
	struct by_extension { };

	struct type_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_name>, mi::member<registered_type, std::string, &registered_type::name>>,
		mi::hashed_unique<mi::tag<by_type>, mi::member<registered_type, std::type_index, &registered_type::type>>,
		mi::hashed_non_unique<mi::tag<by_extension>, mi::member<registered_type, std::string, &registered_type::extension>>
	> { };

	using type_container = mi::multi_index_container<
		registered_type,
		type_container_indices
	>;

private:
	static type_container& s_types;
	static registered_type s_empty_type;

	friend struct type_registry_initializer;
};

namespace detail
{
	template<typename T>
	struct type_registerer
	{
		type_registerer(const std::string& name, const std::string& extension)
		{
			type_registry::registerType(registered_type(typeid(T), name, extension));
		}
	};
}

#define REGISTER_OBJECT_TYPE(c, n, e)						\
namespace													\
{															\
	detail::type_registerer<c> register_type_##c (n, e);	\
}

#define REGISTER_OBJECT_TYPE_NO_EXT(c, n) REGISTER_OBJECT_TYPE(c, n, "")


static struct type_registry_initializer
{
	type_registry_initializer();
	~type_registry_initializer();
} type_registry_init;

#endif // TYPE_REGISTRY_HPP