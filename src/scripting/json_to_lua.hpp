#ifndef SCRIPTING_JSON_TO_LUA_HPP
#define SCRIPTING_JSON_TO_LUA_HPP

#include "nlohmann/json.hpp"

struct lua_State;

namespace hexeract
{
	namespace scripting
	{
		void push_json(lua_State* L, const nlohmann::json& json);
	}
}

#endif // SCRIPTING_JSON_TO_LUA_HPP