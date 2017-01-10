#include "DayNightController.hpp"
#include "graphics/Light.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "core/Input.hpp"
#include "util/component_registry.hpp"

REGISTER_COMPONENT_CLASS(DayNightController, "dayNightController");

json_interpreter<DayNightController> DayNightController::s_properties{
	{ "daySkyColor",			&DayNightController::m_daySkyColor },
	{ "nightSkyColor",			&DayNightController::m_nightSkyColor },
	{ "dayLightColor",			&DayNightController::m_dayLightColor },
	{ "nightLightColor",		&DayNightController::m_nightLightColor },
	{ "dayLightIntensity",		&DayNightController::m_dayLightIntensity },
	{ "nightLightIntensity",	&DayNightController::m_nightLightIntensity }
};

DayNightController::DayNightController(Entity* parent) : Component(parent), m_dayTime(true), m_light(nullptr), m_dayLightIntensity(0.0f), m_nightLightIntensity(0.0f) { }

void DayNightController::start_impl()
{
	m_light = getEntity()->getComponent<Light>();
	applyValues(true);
}

void DayNightController::update_impl()
{
	if (Input::instance()->getKeyPressed(GLFW_KEY_N)) {
		setDayTime(!getDayTime());
	}
}

void DayNightController::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void DayNightController::setDayTime(bool val)
{
	if (val != m_dayTime) {
		m_dayTime = val;
		applyValues(m_dayTime);
	}
}

void DayNightController::applyValues(bool dayTime)
{
	Scene* scene = getEntity()->getParentScene();

	if (scene) {
		glm::vec4 skyColor = dayTime ? m_daySkyColor : m_nightSkyColor;
		scene->setBackColor(skyColor);
		scene->setAmbientLight(skyColor);
	}
	
	if (m_light) {
		m_light->setColor(dayTime ? m_dayLightColor : m_nightLightColor);
		m_light->setIntensity(dayTime ? m_dayLightIntensity : m_nightLightIntensity);
	}
}
