#ifndef SCRIPTING_UTILITY_HPP
#define SCRIPTING_UTILITY_HPP

#include <string>
#include <type_traits>
#include <vector>

#include "lua.hpp"
#include "glm.hpp"
#include "util/bounds.hpp"
#include "boost/polymorphic_pointer_cast.hpp"
#include "json_to_lua.hpp"

class Object;

namespace scripting
{

	inline int pseudo_index(lua_State* L, int i)
	{
		return lua_gettop(L) + i + 1;
	}

	inline void set_field(lua_State* L, int idx, const std::string& k)
	{
		lua_setfield(L, idx, k.c_str());
	}

	inline void set_field(lua_State* L, int idx, const char* k)
	{
		lua_setfield(L, idx, k);
	}

	void load_module(lua_State* L, const std::string& name);

	void raise_error(lua_State* L, const std::string& msg);

	std::string value_to_string(lua_State* L, int idx);
	inline std::string value_typename(lua_State* L, int idx)
	{
		return lua_typename(L, lua_type(L, idx));
	}

	void swap_top_2(lua_State* L);

	Object* get_object(lua_State* L, int idx);
	void push_object(lua_State* L, Object* obj);
	Object* check_object_arg(lua_State* L, int arg);

	template<typename T>
	struct assert_false : std::false_type { };

	template<typename T, typename Enable = void>
	struct type_handler
	{
		static_assert(assert_false<T>::value, "This type is not supported by the scripting environment!");
	};

	template<typename T>
	T get_value(lua_State* L, int idx = -1)
	{
		return T(type_handler<T>::get_value(L, idx));
	}

	template<typename T>
	void push_value(lua_State* L, const T& value)
	{
		type_handler<std::decay_t<T>>::push_value(L, value);
	}

	template<typename T>
	T check_arg(lua_State* L, int arg)
	{
		return T(type_handler<T>::check_arg(L, arg));
	}

	template<typename T>
	T* check_self(lua_State* L, int arg = 1)
	{
		static_assert(is_object<T>::value, "self should be converted to an object type!");
		return check_arg<T*>(L, arg);
	}

	inline void push_values(lua_State* L) { }

	template<typename T, typename... Args>
	void push_values(lua_State* L, T&& value, Args&&... args)
	{
		push_value(L, std::forward<T>(value));
		push_values(L, std::forward<Args>(args)...);
	}

	template<typename ObjT, typename Ret, typename... Args>
	using obj_method = Ret(ObjT::*)(Args...);

	template<typename ObjT, typename Ret, typename... Args>
	using const_obj_method = Ret(ObjT::*)(Args...) const;

	template<typename T, typename Func, typename Ret, typename... Args>
	struct method_caller
	{
		static int call(lua_State* L, T* self, Func method, int firstArg)
		{
			int a = firstArg - 1;
			push_value<Ret>(L, (self->*method)(check_arg<std::decay_t<Args>>(L, ++a)...));
			return 1;
		}
	};

	template<typename T, typename Func, typename... Args>
	struct method_caller<T, Func, void, Args...>
	{
		static int call(lua_State* L, T* self, Func method, int firstArg)
		{
			int a = firstArg - 1;
			(self->*method)(check_arg<std::decay_t<Args>>(L, ++a)...);
			return 0;
		}
	};

	template<typename T, typename Func, typename Ret, typename... Args>
	int call_method_self(lua_State* L, Func method)
	{
		return method_caller<T, Func, Ret, Args...>::call(L, check_self<T>(L), method, 2);
	}

	template<typename T, typename Func, typename Ret, typename... Args>
	int call_method_module(lua_State* L, Func method)
	{
		return method_caller<T, Func, Ret, Args...>::call(L, instance<T>(), method, 1);
	}

	template<typename T, typename Ret, typename... Args>
	int method_self_helper(lua_State* L, obj_method<T, Ret, Args...> method)
	{
		return call_method_self<T, obj_method<T, Ret, Args...>, Ret, Args...>(L, method);
	}

	template<typename T, typename Ret, typename... Args>
	auto method_self_helper(lua_State* L, const_obj_method<T, Ret, Args...> method) -> std::enable_if_t<!std::is_pointer<Ret>::value, int>
	{
		return call_method_self<T, const_obj_method<T, Ret, Args...>, Ret, Args...>(L, method);
	}

	template<typename T, typename Ret, typename... Args>
	int method_module_helper(lua_State* L, obj_method<T, Ret, Args...> method)
	{
		return call_method_module<T, obj_method<T, Ret, Args...>, Ret, Args...>(L, method);
	}

	template<typename T, typename Ret, typename... Args>
	auto method_module_helper(lua_State* L, const_obj_method<T, Ret, Args...> method) -> std::enable_if_t<!std::is_pointer<Ret>::value, int>
	{
		return call_method_module<T, const_obj_method<T, Ret, Args...>, Ret, Args...>(L, method);
	}

	template<>
	struct type_handler<std::nullptr_t>
	{
		static std::nullptr_t get_value(lua_State* L, int idx)
		{
			return nullptr;
		}

		static void push_value(lua_State* L, std::nullptr_t value)
		{
			lua_pushnil(L);
		}

		static std::nullptr_t check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TNIL);
			return nullptr;
		}
	};

	template<>
	struct type_handler<bool>
	{
		static bool get_value(lua_State* L, int idx)
		{
			return lua_toboolean(L, idx);
		}

		static void push_value(lua_State* L, bool value)
		{
			lua_pushboolean(L, value);
		}

		static bool check_arg(lua_State* L, int arg)
		{
			luaL_checkany(L, arg);
			return lua_toboolean(L, arg);
		}
	};

	template<>
	struct type_handler<lua_Unsigned>
	{
		static lua_Unsigned get_value(lua_State* L, int idx)
		{
			return lua_tounsigned(L, idx);
		}

		static void push_value(lua_State* L, lua_Unsigned value)
		{
			lua_pushunsigned(L, value);
		}

		static lua_Unsigned check_arg(lua_State* L, int arg)
		{
			return luaL_checkunsigned(L, arg);
		}
	};

	template<>
	struct type_handler<unsigned int> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<unsigned short> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<unsigned char> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<lua_Integer>
	{
		static lua_Integer get_value(lua_State* L, int idx)
		{
			return lua_tointeger(L, idx);
		}

		static void push_value(lua_State* L, lua_Integer value)
		{
			lua_pushinteger(L, value);
		}

		static lua_Integer check_arg(lua_State* L, int arg)
		{
			return luaL_checkinteger(L, arg);
		}
	};

	template<>
	struct type_handler<int> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<short> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<char> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<lua_Number>
	{
		static lua_Number get_value(lua_State* L, int idx)
		{
			return lua_tonumber(L, idx);
		}

		static void push_value(lua_State* L, lua_Number value)
		{
			lua_pushnumber(L, value);
		}

		static lua_Number check_arg(lua_State* L, int arg)
		{
			return luaL_checknumber(L, arg);
		}
	};

	template<>
	struct type_handler<float> : public type_handler<lua_Number> { };

	template<>
	struct type_handler<char*>
	{
		static const char* get_value(lua_State* L, int idx)
		{
			return lua_tostring(L, idx);
		}

		static void push_value(lua_State* L, const char* value)
		{
			lua_pushstring(L, value);
		}

		static const char* check_arg(lua_State* L, int arg)
		{
			return luaL_checkstring(L, arg);
		}
	};

	template<>
	struct type_handler<std::string>
	{
		static std::string get_value(lua_State* L, int idx)
		{
			return lua_tostring(L, idx);
		}

		static void push_value(lua_State* L, const std::string& value)
		{
			lua_pushstring(L, value.c_str());
		}

		static std::string check_arg(lua_State* L, int arg)
		{
			return luaL_checkstring(L, arg);
		}
	};

	template<typename T>
	using is_object = std::is_base_of<Object, T>;

	template<typename T, typename Enable = void>
	struct ptr_type_handler
	{
		static T* get_value(lua_State* L, int idx)
		{
			return static_cast<T*>(lua_touserdata(L, idx));
		}

		static void push_value(lua_State* L, T* value)
		{
			lua_pushlightuserdata(L, value);
		}

		static T* check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TLIGHTUSERDATA);
			return ptr_type_handler::get_value(L, arg);
		}
	};

	template<typename T>
	struct ptr_type_handler<T, std::enable_if_t<std::is_object<T>::value>>
	{
		static T* get_value(lua_State* L, int idx)
		{
			// NOTE: polymorphic_pointer_downcast only performs runtime checks in debug configs. if this causes too many problems, change this to dynamic_cast
			return boost::polymorphic_pointer_downcast<T>(get_object(L, idx));
		}

		static void push_value(lua_State* L, T* value)
		{
			push_object(L, value);
		}

		static T* check_arg(lua_State* L, int arg)
		{
			// NOTE: polymorphic_pointer_downcast only performs runtime checks in debug configs. if this causes too many problems, change this to dynamic_cast
			T* obj = boost::polymorphic_pointer_downcast<T>(check_object_arg(L, arg));
			if (!obj) {
				luaL_argerror(L, arg, "invalid object pointer");
			}
			return obj;
		}
	};

	template<typename T>
	struct type_handler<T*> : public ptr_type_handler<T>
	{ };

	template<typename T>
	struct type_handler<T, std::enable_if_t<std::is_enum<T>::value>>
	{
		static T get_value(lua_State* L, int idx)
		{
			return static_cast<T>(scripting::get_value<lua_Integer>(L, idx));
		}

		static void push_value(lua_State* L, T value)
		{
			scripting::push_value(L, static_cast<lua_Integer>(value));
		}

		static T check_arg(lua_State* L, int arg)
		{
			return static_cast<T>(scripting::check_arg<lua_Integer>(L, arg));
		}
	};

	std::size_t table_length(lua_State* L, int idx);

	template<typename T>
	void get_array(lua_State* L, int idx, T* ptr, std::size_t count)
	{
		for (std::size_t i = 0; i < count; ++i) {
			lua_geti(L, idx, i + 1);
			ptr[i] = get_value<T>(L, -1);
			lua_pop(L, 1);
		}
	}

	template<typename T>
	void push_array(lua_State* L, const T* ptr, std::size_t count)
	{
		lua_createtable(L, int(count), 0);
		for (std::size_t i = 0; i < count; ++i) {
			scripting::push_value<T>(L, ptr[i]);
			lua_seti(L, -2, i + 1);
		}
	}

	template<typename T>
	struct type_handler<std::vector<T>>
	{
		static std::vector<T> get_value(lua_State* L, int idx)
		{
			auto l = table_length(L, idx);
			std::vector<T> result{l};
			get_array(L, idx, result.data(), l);
			return result;
		}

		static void push_value(lua_State* L, const std::vector<T>& value)
		{
			push_array(L, value.data(), value.size());
		}

		static std::vector<T> check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			// TODO: check type of each element
			return get_value(L, arg);
		}
	};

	template<typename T>
	void push_struct(lua_State* L, const std::string& cn, const T* ptr, std::size_t size)
	{
		load_module(L, cn.c_str());
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		for (std::size_t i = 0; i < size; ++i) {
			push_value<T>(L, ptr[i]);
		}
		lua_call(L, int(size), 1);
	}

	template<typename T, glm::precision p>
	struct type_handler<glm::tvec2<T, p>>
	{
		static glm::tvec2<T, p> get_value(lua_State* L, int idx)
		{
			glm::tvec2<T, p> result;
			get_array(L, idx, &result[0], 2);
			return result;
		}

		static void push_value(lua_State* L, const glm::tvec2<T, p>& value)
		{
			push_struct(L, "xmath.vec2", &value[0], 2);
		}

		static glm::tvec2<T, p> check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			auto l = table_length(L, arg);
			luaL_argcheck(L, l >= 2, arg, "expected vector with 2 elements");
			return get_value(L, arg);
		}
	};

	template<typename T, glm::precision p>
	struct type_handler<glm::tvec3<T, p>>
	{
		static glm::tvec3<T, p> get_value(lua_State* L, int idx)
		{
			glm::tvec3<T, p> result;
			get_array(L, idx, &result[0], 3);
			return result;
		}

		static void push_value(lua_State* L, const glm::tvec3<T, p>& value)
		{
			push_struct(L, "xmath.vec3", &value[0], 3);
		}

		static glm::tvec3<T, p> check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			auto l = table_length(L, arg);
			luaL_argcheck(L, l >= 3, arg, "expected vector with 3 elements");
			return get_value(L, arg);
		}
	};

	template<typename T, glm::precision p>
	struct type_handler<glm::tvec4<T, p>>
	{
		static glm::tvec4<T, p> get_value(lua_State* L, int idx)
		{
			glm::tvec4<T, p> result;
			get_array(L, idx, &result[0], 4);
			return result;
		}

		static void push_value(lua_State* L, const glm::tvec4<T, p>& value)
		{
			push_struct(L, "xmath.vec4", &value[0], 4);
		}

		static glm::tvec4<T, p> check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			auto l = table_length(L, arg);
			luaL_argcheck(L, l >= 4, arg, "expected vector with 4 elements");
			return get_value(L, arg);
		}
	};

	template<>
	struct type_handler<aabb>
	{
		static aabb get_value(lua_State* L, int idx)
		{
			lua_getfield(L, idx, "min");
			lua_getfield(L, idx, "max");
			aabb result{
				scripting::get_value<glm::vec3>(L, -2),
				scripting::get_value<glm::vec3>(L, -1)
			};
			lua_pop(L, 2);
			return result;
		}

		static void push_value(lua_State* L, const aabb& value)
		{
			load_module(L, "xmath.aabb");
			lua_getfield(L, -1, "new");
			lua_remove(L, -2);
			scripting::push_value<glm::vec3>(L, value.min);
			scripting::push_value<glm::vec3>(L, value.max);
			lua_call(L, 2, 1);
		}

		static aabb check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			return get_value(L, arg);
		}
	};


	template<typename T, glm::precision p>
	struct type_handler<glm::tquat<T, p>>
	{
		static glm::tquat<T, p> get_value(lua_State* L, int idx)
		{
			glm::tquat<T, p> result;
			get_array(L, idx, &result[0], 4);
			return result;
		}

		static void push_value(lua_State* L, const glm::tquat<T, p>& value)
		{
			push_struct(L, "xmath.quat", &value[0], 4);
		}

		static glm::tquat<T, p> check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TTABLE);
			auto l = table_length(L, arg);
			luaL_argcheck(L, l >= 4, arg, "expected quaternion with 4 elements");
			return get_value(L, arg);
		}
	};

	template<>
	struct type_handler<nlohmann::json>
	{
		static void push_value(lua_State* L, const nlohmann::json& value)
		{
			push_json(L, value);
		}
	};
}

#endif // SCRIPTING_UTILITY_HPP
