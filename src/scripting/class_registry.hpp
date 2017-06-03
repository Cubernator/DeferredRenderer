#ifndef SCRIPTING_CLASS_REGISTRY_HPP
#define SCRIPTING_CLASS_REGISTRY_HPP

#include "utility.hpp"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"

#include <vector>

namespace mi = boost::multi_index;

namespace scripting
{
	struct registered_class
	{
		using method = std::pair<std::string, lua_CFunction>;

		std::string name, baseName;
		std::vector<method> methods;

		registered_class(const std::string& name, const std::string& baseName) : name(name), baseName(baseName) { }

		void addMethod(const std::string& methodName, lua_CFunction f) { methods.push_back({ methodName, f }); }

		void apply() const;
	};

	class class_registry
	{
	public:
		static void registerClass(const std::string& name, const std::string& baseName);
		static void registerMethod(const std::string& className, const std::string& methodName, lua_CFunction f);

		static void applyAllClasses();

		struct class_container_indices : public mi::indexed_by<
			mi::hashed_unique<mi::member<registered_class, std::string, &registered_class::name>>
		> { };
		using class_container = mi::multi_index_container<
			registered_class,
			class_container_indices
		>;

	private:
		static class_container& s_classes;

		friend struct class_registry_initializer;
	};

	namespace detail
	{
		struct class_registerer
		{
			class_registerer(const std::string& name, const std::string& baseName)
			{
				class_registry::registerClass(name, baseName);
			}
		};

		struct method_registerer
		{
			method_registerer(const std::string& className, const std::string& methodName, lua_CFunction f)
			{
				class_registry::registerMethod(className, methodName, f);
			}
		};
	}

	static struct class_registry_initializer
	{
		class_registry_initializer();
		~class_registry_initializer();
	} class_registry_init;
}


#define SCRIPTING_REGISTER_DERIVED_CLASS(c, b)						\
namespace															\
{																	\
	scripting::detail::class_registerer register_class##c (#c, #b);	\
}


#define SCRIPTING_REGISTER_CLASS(c) SCRIPTING_REGISTER_DERIVED_CLASS(c, Object)

#define SCRIPTING_CALLBACK_NAME(c, n) lua_##c##_##n
#define SCRIPTING_CALLBACK_SIG(c, n) int SCRIPTING_CALLBACK_NAME(c, n) (lua_State* L)


#define SCRIPTING_DEFINE_METHOD(c, n) SCRIPTING_CALLBACK_SIG(c, n);										\
namespace																								\
{																										\
	scripting::detail::method_registerer register_method##c##n(#c, #n, (SCRIPTING_CALLBACK_NAME(c, n)));\
}																										\
SCRIPTING_CALLBACK_SIG(c, n)


#define SCRIPTING_DEFINE_GETTER(c, n, m) SCRIPTING_DEFINE_METHOD(c, n) { return scripting::getter_helper(L, &c::m); }
#define SCRIPTING_DEFINE_SETTER(c, n, m) SCRIPTING_DEFINE_METHOD(c, n) { return scripting::setter_helper(L, &c::m); }

#endif // SCRIPTING_CLASS_REGISTRY_HPP