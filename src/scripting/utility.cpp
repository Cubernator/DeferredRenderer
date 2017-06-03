#include "utility.hpp"

#include "lua.hpp"
#include "boost/format.hpp"

#include <iostream>

namespace scripting
{
	void raise_error(lua_State* L, const std::string& msg)
	{
		lua_pushstring(L, msg.c_str());
		lua_error(L);
	}

	void stack_dump(lua_State* L)
	{
		using fmt = boost::format;

		int top = lua_gettop(L);
		for (int i = 1; i <= top; ++i) {
			std::cout << (fmt("[%1$2i] ") % i);
			fmt o;

			int t = lua_type(L, i);
			switch (t) {
			case LUA_TNIL:
				o = fmt("     nil");
				break;
			case LUA_TBOOLEAN:
				o = fmt(" boolean: %1%") % (lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				o = fmt("  number: %1%") % lua_tonumber(L, i);
				break;
			case LUA_TSTRING:
				o = fmt("  string: \"%1%\"") % lua_tostring(L, i);
				break;
			default:
				o = fmt("%1$8s: 0x%2%") % lua_typename(L, t) % lua_topointer(L, i);
				break;
			}

			std::cout << o << std::endl;
		}
	}

	void swap_top_2(lua_State* L)
	{
		auto i1 = pseudo_index(L, -2), i2 = pseudo_index(L, -1);
		lua_pushvalue(L, i1);
		lua_copy(L, i2, i1);
		lua_replace(L, i2);
	}

	Object* get_object(lua_State* L, int idx)
	{
		lua_getfield(L, idx, "_ptr");
		Object* obj = static_cast<Object*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		return obj;
	}

	void push_object(lua_State* L, Object* obj)
	{
		lua_getglobal(L, "_objects");
		lua_pushlightuserdata(L, obj);
		lua_gettable(L, -2);
		lua_remove(L, -2);
	}

	Object* check_object_arg(lua_State* L, int arg)
	{
		luaL_checktype(L, arg, LUA_TTABLE);
		return get_object(L, arg);
	}
}