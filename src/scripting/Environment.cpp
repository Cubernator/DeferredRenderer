#include "Environment.hpp"
#include "Behaviour.hpp"
#include "class_registry.hpp"
#include "content/Registry.hpp"
#include "core/app_info.hpp"
#include "core/Component.hpp"

#include "boost/format.hpp"

#include "lua.hpp"

namespace hexeract
{
	namespace scripting
	{
		int lua_destroy(lua_State* L)
		{
			destroy_object(check_object_arg(L, 1));
			return 0;
		}

		Environment::Environment() : m_lg("Scripting")
		{
			LOG_INFO(m_lg) << "Creating lua state...";
			m_L = luaL_newstate();

			lua_atpanic(m_L, &Environment::onPanic);

			luaL_openlibs(m_L);

			lua_getglobal(m_L, "package");
			lua_getfield(m_L, -1, "path");
			std::string luaPath = lua_tostring(m_L, -1);

			path croot = content::Registry::instance()->contentRoot();
			auto searchDirs = app_info::get<std::vector<path>>("scriptSearchDirs");

			auto fmt = boost::format(";./%1%/?.lua;./%1%/?/init.lua");

			for (path& dir : searchDirs) {
				luaPath += (fmt % (croot / dir).generic_string()).str();
			}
			LOG_DEBUG(m_lg) << "Lua path: " << luaPath;

			lua_pop(m_L, 1);
			lua_pushstring(m_L, luaPath.c_str());
			lua_setfield(m_L, -2, "path");
			lua_pop(m_L, 1);

			loadModule("Object");
			lua_register(m_L, "destroy", lua_destroy);

			LOG_INFO(m_lg) << "Registering classes and methods...";
			class_registry::applyAllClasses();
		}

		Environment::~Environment()
		{
			lua_close(m_L);
		}

		bool Environment::loadModule(const std::string& name)
		{
			lua_getglobal(m_L, "require");
			lua_pushstring(m_L, name.c_str());
			return !safeCall(1, 0);
		}

		void Environment::addClass(const std::string& name, const std::string& base)
		{
			LOG_DEBUG(m_lg) << "Adding class \"" << name << "\" (Derived from \"" << base << "\")";
			instantiateClass(base, nullptr);
			lua_setglobal(m_L, name.c_str());
		}

		void Environment::addClass(const std::string& name)
		{
			addClass(name, "Object");
		}

		void Environment::addStaticClass(const std::string& name)
		{
			LOG_DEBUG(m_lg) << "Adding static class \"" << name << "\"";
			lua_newtable(m_L);
			lua_setglobal(m_L, name.c_str());
		}

		void Environment::instantiateClass(const std::string& className, Object* obj)
		{
			lua_getglobal(m_L, className.c_str());
			if (!lua_istable(m_L, -1)) {
				pop();
				LOG_WARNING(m_lg) << "Could not find class \"" << className << "\"! Instantiating as \"Object\" instead.";
				lua_getglobal(m_L, "Object");
			}

			lua_getfield(m_L, -1, "new");
			swap_top_2(m_L);
			safeCall(1, 1);

			if (obj) {
				// store object pointer (lua -> C++)
				lua_pushlightuserdata(m_L, obj);
				lua_setfield(m_L, -2, "_ptr");

				// register lua object in global table with pointer as key (C++ -> lua)
				lua_getglobal(m_L, "_objects");
				lua_pushlightuserdata(m_L, obj);
				lua_pushvalue(m_L, -3);
				lua_settable(m_L, -3);
				pop();
			}
		}

		void Environment::invalidateObject(Object* obj)
		{
			lua_getglobal(m_L, "_objects");
			lua_pushlightuserdata(m_L, obj);
			lua_gettable(m_L, -2);

			if (!lua_istable(m_L, -1)) {
				pop();
				LOG_ERROR(m_lg) << "Failed to invalidate object: " << obj;
				return;
			}

			// invalidate object pointer
			lua_pushlightuserdata(m_L, nullptr);
			lua_setfield(m_L, -2, "_ptr");

			// remove object from global table
			lua_pushlightuserdata(m_L, obj);
			lua_pushnil(m_L);
			lua_settable(m_L, -4);
			pop(2);
		}

		void Environment::pushObject(Object* obj)
		{
			push_object(m_L, obj);
		}

		int Environment::safeCall(int argc, int resc)
		{
			int base = lua_gettop(m_L) - argc;
			lua_pushcfunction(m_L, &Environment::traceback);
			lua_insert(m_L, base);
			int status = lua_pcall(m_L, argc, resc, base);
			if (status != LUA_OK) {
				const char* msg = lua_tostring(m_L, -1);
				LOG_ERROR(m_lg) << msg;
				pop();
			}
			lua_remove(m_L, base);
			return status;
		}

		void Environment::callBehaviourMethod(Behaviour* bhv, const std::string& methodName)
		{
			if (bhv->isGood()) {
				callMethod(bhv, methodName, 0);
			}
		}

		void Environment::addComponent(Component* cmpt)
		{
			auto b = dynamic_cast<Behaviour*>(cmpt);
			if (b) {
				m_addBhvs.push_back(b);
			}
		}

		void Environment::removeComponent(Component* cmpt)
		{
			auto b = dynamic_cast<Behaviour*>(cmpt);
			if (b) {
				m_removeBhvs.push_back(b);
			}
		}

		void Environment::update()
		{
			while (m_addBhvs.size() > 0) {
				m_behaviours.insert(m_behaviours.end(), m_addBhvs.begin(), m_addBhvs.end());
				auto abs = std::move(m_addBhvs);

				for (auto b : abs) {
					callBehaviourMethod(b, "init");
				}
			}

			for (auto rb : m_removeBhvs) {
				m_behaviours.erase(std::remove(m_behaviours.begin(), m_behaviours.end(), rb), m_behaviours.end());
			}
			m_removeBhvs.clear();

			// TODO: call start functions (on the first frame a behaviour is enabled)

			for (auto b : m_behaviours) {
				if (b->isActiveAndEnabled()) {
					callBehaviourMethod(b, "update");
				}
			}
		}

		int Environment::traceback(lua_State* L)
		{
			std::string msg;
			int hasMeta = luaL_callmeta(L, -1, "__tostring");

			if (lua_isstring(L, -1)) {
				msg = lua_tostring(L, -1);
			} else {
				msg = value_typename(L, -1) + ": " + value_to_string(L, -1);
			}

			if (hasMeta)
				lua_pop(L, 1);

			luaL_traceback(L, L, msg.c_str(), 1);
			return 1;
		}

		void Environment::stackDump(lua_State* L)
		{
			int top = lua_gettop(L);
			for (int i = 1; i <= top; ++i) {
				LOG_INFO(instance()->m_lg) << (boost::format("[%1$2i] %2$8s: %3%")
					% i
					% value_typename(L, i)
					% value_to_string(L, i));
			}
		}

		int Environment::onPanic(lua_State* L)
		{
			const char* msg = lua_tostring(L, -1);
			lua_pop(L, 1);

			LOG_ERROR(instance()->m_lg) << "Unprotected error in lua API: " << msg;
			LOG_INFO(instance()->m_lg) << "stack contents:";
			stackDump(L);

			return 0;
		}
	}
}