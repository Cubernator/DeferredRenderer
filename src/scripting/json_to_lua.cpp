#include "json_to_lua.hpp"

#include "utility.hpp"
#include "Behaviour.hpp"
#include "util/json_utils.hpp"
#include "keyword_helper.hpp"
#include "content/pooled.hpp"
#include "core/Entity.hpp"
#include "core/component_registry.hpp"

#include <functional>

namespace scripting
{

	using value_t = nlohmann::json::value_t;
	using pusher_function = std::function<void(lua_State*, const nlohmann::json&)>;

	struct object_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			NamedObject* obj = nullptr;
			if (json.is_array() && (json.size() >= 2)) {
				const auto& t = json.at(0);
				if (t.is_string()) {
					const auto& v = json.at(1);
					obj = content::get_pooled_json(t.get<std::string>(), v);
				}
			}
			push_object(L, obj);
		}
	};

	struct cmpt_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			Component* cmpt = nullptr;
			if (json.is_array() && (json.size() >= 2)) {
				const auto& en = json.at(0);
				if (en.is_string()) {
					const auto& ct = json.at(1);
					auto entity = instance<ObjectRegistry>()->findTNFirst<Entity>(en);
					if (entity) {
						auto& fact = component_registry::findByName(ct);
						cmpt = fact.factory->getFirst(entity);
					}
				}
			}
			push_object(L, cmpt);
		}
	};

	struct bhv_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			Behaviour* bhv = nullptr;
			if (json.is_array() && (json.size() >= 2)) {
				const auto& en = json.at(0);
				if (en.is_string()) {
					const auto& bt = json.at(1);
					auto entity = instance<ObjectRegistry>()->findTNFirst<Entity>(en);
					if (entity) {
						bhv = entity->getBehaviour(bt);
					}
				}
			}
			push_object(L, bhv);
		}
	};

	template<typename T>
	struct json_value_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			push_value<T>(L, json_get<T>(json));
		}
	};

	namespace
	{
		keyword_helper<pusher_function> g_named_types{
			{ "object", object_pusher() },
			{ "component", cmpt_pusher() },
			{ "behaviour", bhv_pusher() },
			{ "aabb", json_value_pusher<aabb>() },
			{ "color", json_value_pusher<glm::vec4>() },
			{ "quat", json_value_pusher<glm::quat>() },
			{ "vec2", json_value_pusher<glm::vec2>() },
			{ "vec3", json_value_pusher<glm::vec3>() },
			{ "vec4", json_value_pusher<glm::vec4>() }
		};
	}

	struct json_object_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			std::string type;
			if (::get_value(json, "type", type)) {
				auto it = json.find("value");
				if (it != json.end()) {
					pusher_function f;
					if (g_named_types.get(type, f)) {
						f(L, *it);
					} else {
						lua_pushnil(L);
					}
					return;
				}
			}

			lua_createtable(L, 0, int(json.size()));
			for (auto it = json.begin(); it != json.end(); ++it) {
				push_json(L, it.value());
				lua_setfield(L, -2, it.key().c_str());
			}
		}
	};

	struct json_array_pusher
	{
		void operator() (lua_State* L, const nlohmann::json& json) const
		{
			lua_createtable(L, int(json.size()), 0);
			for (std::size_t i = 0; i < json.size(); ++i) {
				push_json(L, json.at(i));
				lua_seti(L, -2, lua_Integer(i));
			}
		}
	};

	namespace
	{
		keyword_helper<pusher_function, value_t> g_json_to_lua{
			{ value_t::null,			json_value_pusher<std::nullptr_t>()	},
			{ value_t::boolean,			json_value_pusher<bool>()			},
			{ value_t::number_integer,	json_value_pusher<lua_Integer>()	},
			{ value_t::number_unsigned,	json_value_pusher<lua_Unsigned>()	},
			{ value_t::number_float,	json_value_pusher<lua_Number>()		},
			{ value_t::string,			json_value_pusher<std::string>()	},
			{ value_t::object,			json_object_pusher()				},
			{ value_t::array,			json_array_pusher()					}
		};
	}

	void push_json(lua_State* L, const nlohmann::json& json)
	{
		pusher_function f;
		if (g_json_to_lua.get(json.type(), f)) {
			f(L, json);
		}
	}
}
