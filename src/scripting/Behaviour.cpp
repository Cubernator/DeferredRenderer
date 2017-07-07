#include "Behaviour.hpp"
#include "Environment.hpp"
#include "core/Entity.hpp"
#include "core/component_registry.hpp"

#include "lua.hpp"
#include "json_to_lua.hpp"
#include "class_registry.hpp"

namespace scripting
{
	REGISTER_COMPONENT_CLASS(Behaviour);

	Behaviour::Behaviour(Entity* parent) : Component(parent), m_good(false), m_objIdx(0) { }

	Behaviour::~Behaviour()
	{
		destroyObj();
	}

	void Behaviour::destroyObj()
	{
		if (m_good) {
			Environment::instance()->invalidateObject(this);
			m_good = false;
		}
	}

	void Behaviour::setScript(const std::string& name)
	{
		destroyObj();
		m_script = name;

		auto scriptEnv = Environment::instance();
		auto L = scriptEnv->state();

		m_good = scriptEnv->loadModule(m_script);
		if (!m_good) return;

		scriptEnv->createObject(m_script, this, false);
		m_good = lua_istable(L, -1);
		scriptEnv->pop();
	}

	void Behaviour::apply_json_impl(const nlohmann::json& json)
	{
		Component::apply_json_impl(json);

		std::string scriptName;
		if (::get_value(json, "script", scriptName)) {
			setScript(scriptName);

			if (m_good) {
				auto it = json.find("properties");
				if (it != json.end() && it->is_object()) {
					auto L = getState();

					beginProperties(L);
					for (auto pit = it->begin(); pit != it->end(); ++pit) {
						push_json(L, pit.value());
						submitProperty(L, pit.key());
					}
					endProperties(L);
				}
			}
		}
	}

	void Behaviour::beginProperties(lua_State* L)
	{
		push_object(L, this);
		m_objIdx = lua_gettop(L);
	}

	void Behaviour::endProperties(lua_State* L)
	{
		lua_remove(L, m_objIdx);
	}

	void Behaviour::submitProperty(lua_State* L, const std::string& name)
	{
		lua_setfield(L, m_objIdx, name.c_str());
	}

	lua_State* Behaviour::getState()
	{
		return instance<Environment>()->state();
	}
}

SCRIPTING_REGISTER_DERIVED_CLASS(Behaviour, Component)

SCRIPTING_DEFINE_METHOD(Behaviour, init)
{
	return 0;
}

SCRIPTING_DEFINE_METHOD(Behaviour, start)
{
	return 0;
}

SCRIPTING_DEFINE_METHOD(Behaviour, update)
{
	return 0;
}
