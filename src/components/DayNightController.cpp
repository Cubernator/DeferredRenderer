#include "DayNightController.hpp"
#include "graphics/Light.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "input/Input.hpp"
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

DayNightController::DayNightController(Entity* parent) : Component(parent), m_dayTime(true), m_ambient(true), m_light(nullptr),
m_dayLightIntensity(0.0f), m_nightLightIntensity(0.0f) { }

void DayNightController::start_impl()
{
	m_light = getEntity()->getComponent<Light>();
	applyValues();
}

void DayNightController::update_impl()
{
	auto input = Input::instance();

	if (input->getKeyPressed(key_n)) {
		setDayTime(!getDayTime());
	}

	if (input->getKeyPressed(key_b)) {
		setAmbient(!getAmbient());
	}
}

void DayNightController::apply_json_impl(const nlohmann::json& json)
{
	Component::apply_json_impl(json);
	s_properties.interpret_all(this, json);
}

void DayNightController::setDayTime(bool val)
{
	if (val != m_dayTime) {
		m_dayTime = val;
		applyValues();
	}
}

void DayNightController::setAmbient(bool val)
{
	if (val != m_ambient) {
		m_ambient = val;
		applyValues();
	}
}

void DayNightController::applyValues()
{
	Scene* scene = getEntity()->getParentScene();

	if (scene) {
		glm::vec4 skyColor = m_ambient ? (m_dayTime ? m_daySkyColor : m_nightSkyColor) : glm::vec4(0.f);
		scene->setBackColor(skyColor);
		scene->setAmbientLight(skyColor);
	}
	
	if (m_light) {
		m_light->setColor(m_dayTime ? m_dayLightColor : m_nightLightColor);
		m_light->setIntensity(m_dayTime ? m_dayLightIntensity : m_nightLightIntensity);
	}
}
