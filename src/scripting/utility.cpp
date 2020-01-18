#include "utility.hpp"

#include "lua.hpp"
#include "boost/format.hpp"

#include <iostream>

namespace hexeract
{
	namespace scripting
	{
		void load_module(lua_State* L, const std::string& name)
		{
			lua_getglobal(L, "require");
			lua_pushstring(L, name.c_str());
			lua_call(L, 1, 1);
		}

		void raise_error(lua_State* L, const std::string& msg)
		{
			lua_pushstring(L, msg.c_str());
			lua_error(L);
		}

		std::string value_to_string(lua_State* L, int idx)
		{
			using fmt = boost::format;

			int t = lua_type(L, idx);
			switch (t) {
			case LUA_TNIL:
				return "nil";
			case LUA_TBOOLEAN:
				return lua_toboolean(L, idx) ? "true" : "false";
			case LUA_TNUMBER:
				return (fmt("%1%") % lua_tonumber(L, idx)).str();
			case LUA_TSTRING:
				return (fmt("\"%1%\"") % lua_tostring(L, idx)).str();
			default:
				return (fmt("0x%1%") % lua_topointer(L, idx)).str();
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

		std::size_t table_length(lua_State* L, int idx)
		{
			lua_len(L, idx);
			auto l = lua_tounsigned(L, -1);
			lua_pop(L, 1);
			return l;
		}
	}
}