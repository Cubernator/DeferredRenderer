#include "Behaviour.hpp"
#include "Environment.hpp"
#include "core/Entity.hpp"
#include "util/component_registry.hpp"

#include "lua.hpp"

namespace scripting
{
	REGISTER_COMPONENT_CLASS(Behaviour, "scriptBehaviour");

	Behaviour::Behaviour(Entity* parent) : Component(parent) { }

	Behaviour::~Behaviour()
	{
		Environment::instance()->invalidateObject(this);
	}

	void Behaviour::start_impl()
	{
		Environment::instance()->callMethod(this, "start", 0);
	}

	void Behaviour::update_impl()
	{
		Environment::instance()->callMethod(this, "update", 0);
	}

	void Behaviour::apply_json_impl(const nlohmann::json& json)
	{
		Component::apply_json_impl(json);

		if (::get_value(json, "script", m_behaviourName)) {
			auto scriptEnv = Environment::instance();

			scriptEnv->loadModule(m_behaviourName);
			scriptEnv->createObject(m_behaviourName, this, false);

			auto it = json.find("properties");
			if (it != json.end() && it->is_object()) {
				for (auto pit = it->begin(); pit != it->end(); ++pit) {
					// apply property to behaviour
				}
			}

			scriptEnv->pop();
		}
	}
}