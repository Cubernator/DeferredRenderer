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

			class_registerer(const std::string& name)
			{
				class_registry::registerClass(name, "");
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

#define SCRIPTING_REGISTER_STATIC_CLASS(className)						\
namespace														\
{																\
	scripting::detail::class_registerer register_class##className (#className);	\
}

#define SCRIPTING_REGISTER_DERIVED_CLASS(className, baseName)						\
namespace															\
{																	\
	scripting::detail::class_registerer register_class##className (#className, #baseName);	\
}


#define SCRIPTING_REGISTER_CLASS(className) SCRIPTING_REGISTER_DERIVED_CLASS(className, Object)

#define SCRIPTING_CALLBACK_NAME(className, methodName) lua_##className##_##methodName
#define SCRIPTING_CALLBACK_SIG(className, methodName) int SCRIPTING_CALLBACK_NAME(className, methodName) (lua_State* L)


#define SCRIPTING_DEFINE_METHOD(className, methodName) SCRIPTING_CALLBACK_SIG(className, methodName);	\
namespace																								\
{																										\
	scripting::detail::method_registerer register_method_##className##_##methodName(#className, #methodName, (SCRIPTING_CALLBACK_NAME(className, methodName)));\
}																										\
SCRIPTING_CALLBACK_SIG(className, methodName)

#define SCRIPTING_AUTO_METHOD_CM(className, methodName, class, method) SCRIPTING_DEFINE_METHOD(className, methodName) { return scripting::method_self_helper(L, &class::method); }
#define SCRIPTING_AUTO_METHOD_C(className, methodName, class) SCRIPTING_AUTO_METHOD_CM(className, methodName, class, methodName)
#define SCRIPTING_AUTO_METHOD_M(className, methodName, method) SCRIPTING_AUTO_METHOD_CM(className, methodName, className, method)
#define SCRIPTING_AUTO_METHOD(className, methodName) SCRIPTING_AUTO_METHOD_CM(className, methodName, className, methodName)

#define SCRIPTING_AUTO_MODULE_METHOD_CM(className, methodName, class, method) SCRIPTING_DEFINE_METHOD(className, methodName) { return scripting::method_module_helper(L, &class::method); }
#define SCRIPTING_AUTO_MODULE_METHOD_C(className, methodName, class) SCRIPTING_AUTO_MODULE_METHOD_CM(className, methodName, class, methodName)
#define SCRIPTING_AUTO_MODULE_METHOD_M(className, methodName, method) SCRIPTING_AUTO_MODULE_METHOD_CM(className, methodName, className, method)
#define SCRIPTING_AUTO_MODULE_METHOD(className, methodName) SCRIPTING_AUTO_MODULE_METHOD_CM(className, methodName, className, methodName)

#endif // SCRIPTING_CLASS_REGISTRY_HPP