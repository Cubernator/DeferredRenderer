#include "class_registry.hpp"
#include "Environment.hpp"

#include <new>
#include <unordered_set>

namespace scripting
{
	using class_container = class_registry::class_container;

	static std::aligned_storage_t<sizeof(class_container), alignof(class_container)> g_classes_buf;
	class_container& class_registry::s_classes = reinterpret_cast<class_container&>(g_classes_buf);

	void class_registry::registerClass(const std::string& name, const std::string& baseName)
	{
		s_classes.insert(registered_class(name, baseName));
	}

	void class_registry::registerMethod(const std::string& className, const std::string& methodName, lua_CFunction f)
	{
		auto it = s_classes.find(className);
		if (it != s_classes.end()) {
			s_classes.modify(it, [&](registered_class& rc) {
				rc.addMethod(methodName, f);
			});
		}
	}

	void class_registry::applyAllClasses()
	{
		std::unordered_set<std::string> classes;

		// keep iterating registered classes until all of them have been applied
		while (classes.size() < s_classes.size()) {
			for (const registered_class& c : s_classes) {

				// only apply class if base class has already been applied
				if (classes.count(c.baseName) || (c.baseName == "Object")) {

					// try inserting class
					auto p = classes.insert(c.name);

					// only apply class if it has not been applied before (if insertion was successful)
					if (p.second) {
						c.apply();
					}
				}
			}
		}
	}

	void registered_class::apply() const
	{
		auto se = Environment::instance();

		se->addClass(name, baseName);
		se->addMethods(name, methods.begin(), methods.end());
	}


	static int schwarz_counter;

	class_registry_initializer::class_registry_initializer()
	{
		if (schwarz_counter++ == 0) new (&class_registry::s_classes) class_container();
	}

	class_registry_initializer::~class_registry_initializer()
	{
		if (--schwarz_counter == 0) (&class_registry::s_classes)->~class_container();
	}
}
