#include "Environment.hpp"
#include "content/Content.hpp"
#include "core/Entity.hpp"
#include "core/Component.hpp"
#include "util/app_info.hpp"

#include "lua.hpp"

namespace scripting
{
	Environment* Environment::s_instance = nullptr;

	Environment::Environment()
	{
		s_instance = this;

		m_L = luaL_newstate();

		lua_atpanic(m_L, &Environment::onPanic);
		lua_pushcfunction(m_L, &Environment::traceback);

		luaL_openlibs(m_L);

		lua_getglobal(m_L, "package");
		lua_getfield(m_L, -1, "path");
		std::string luaPath = lua_tostring(m_L, -1);

		path croot = Content::instance()->contentRoot();
		auto searchDirs = app_info::get<std::vector<path>>("scriptSearchDirs");
		for (path& dir : searchDirs) {
			luaPath += ";./" + (croot / dir).generic_string() + "/?.lua";
		}

		lua_pop(m_L, 1);
		lua_pushstring(m_L, luaPath.c_str());
		lua_setfield(m_L, -2, "path");
		lua_pop(m_L, 1);

		loadModule("Object");

		lua_newtable(m_L);
		lua_setglobal(m_L, "_objects");

		Entity::registerScriptClass();
		Component::registerScriptClass();
	}

	Environment::~Environment()
	{
		lua_close(m_L);
	}

	int Environment::loadModule(const std::string& name)
	{
		lua_getglobal(m_L, "require");
		lua_pushstring(m_L, name.c_str());
		return pcall(1, 0);
	}

	void Environment::addClass(const std::string& name, const std::string& base)
	{
		instantiateClass(base, nullptr);
		lua_setglobal(m_L, name.c_str());
	}

	void Environment::addClass(const std::string& name)
	{
		addClass(name, "Object");
	}

	void Environment::addMethods(const std::string& className, std::initializer_list<std::pair<std::string, lua_CFunction>> methods)
	{
		lua_getglobal(m_L, className.c_str());

		for (auto m : methods) {
			lua_pushcfunction(m_L, m.second);
			lua_setfield(m_L, -2, m.first.c_str());
		}

		pop();
	}

	void Environment::instantiateClass(const std::string& className, Object* obj)
	{
		lua_getglobal(m_L, className.c_str());
		lua_getfield(m_L, -1, "new");
		swap_top_2(m_L);
		pcall(1, 1);

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
			std::cout << "ERROR: failed to invalidate object: " << obj << std::endl;
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

	int Environment::pcall(int argc, int resc)
	{
		int status = lua_pcall(m_L, argc, resc, 1);
		if (status != LUA_OK) {
			const char* msg = lua_tostring(m_L, -1);
			std::cout << "ERROR: " << msg << std::endl;
			pop();
		}
		return status;
	}

	int Environment::traceback(lua_State* L)
	{
		// TODO
		return 1;
	}

	int Environment::onPanic(lua_State* L)
	{
		const char* msg = lua_tostring(L, -1);
		lua_pop(L, 1);

		std::cout << "ERROR: unprotected error in lua API: " << msg << std::endl;
		std::cout << "stack contents:" << std::endl;
		stack_dump(L);

		return 0;
	}
}