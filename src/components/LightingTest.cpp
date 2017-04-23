#include "LightingTest.hpp"
#include "RandomMovement.hpp"
#include "util/component_registry.hpp"
#include "util/random.hpp"
#include "graphics/RenderEngine.hpp"
#include "graphics/Light.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "input/Input.hpp"

#include <fstream>

REGISTER_COMPONENT_CLASS(LightingTest, "lightingTest");

json_interpreter<LightingTest> LightingTest::s_properties{
	{ "lightCount",		&LightingTest::m_lightCount },
	{ "lightArea",		&LightingTest::m_lightArea },
	{ "minSaturation",	&LightingTest::m_minSaturation },
	{ "maxSaturation",	&LightingTest::m_maxSaturation },
	{ "minIntensity",	&LightingTest::m_minIntensity },
	{ "maxIntensity",	&LightingTest::m_maxIntensity },
	{ "rangeFactor",	&LightingTest::m_rangeFactor },
	{ "minSpeed",		&LightingTest::m_minSpeed },
	{ "maxSpeed",		&LightingTest::m_maxSpeed },
	{ "testDuration",	&LightingTest::m_testDuration }
};


LightingTest::LightingTest(Entity* parent) : Component(parent), m_lightsEnabled(false), m_lightsMoving(false), m_lightCount(0),
	m_minSaturation(0.f), m_maxSaturation(1.f),
	m_minIntensity(0.f), m_maxIntensity(1.f),
	m_rangeFactor(1.f),
	m_minSpeed(0.f), m_maxSpeed(1.f),
	m_testDuration(1.0), m_testTimer(0.0), m_testLight(0), m_testing(false), m_testLights(false)
{ }

void LightingTest::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void LightingTest::start_impl()
{
	addLights(m_lightCount);
}

void LightingTest::update_impl()
{
	auto engine = Engine::instance();
	auto input = Input::instance();

	if (m_testing) {
		auto ft = engine->getFrameTime();
		m_accFrameTime += ft;
		++m_testSamples;

		auto time = engine->getTime();

		if (time >= m_testTimer) {
			time_type avgFrameTime = m_accFrameTime / m_testSamples;
			time_type avgFPS = (m_testSamples) / (time - (m_testTimer - m_testDuration));

			m_ftResults.push_back(avgFrameTime);
			m_fpsResults.push_back(avgFPS);
			m_lightsPerObj.push_back(RenderEngine::instance()->getAvgLightsPerObj());
			std::cout << (m_testLight + 1) << " lights: " << avgFrameTime << "ms (" << avgFPS << " FPS)" << std::endl;

			m_testSamples = 0;
			m_accFrameTime = 0.0;
			if (m_testLights) {
				if (m_testLight < m_lights.size()) {
					m_lights[m_testLight]->setActive(true);
					m_testTimer = time + m_testDuration;
					++m_testLight;
				} else {
					m_testing = false;
				}
			} else {
				m_testing = false;
			}

			if (!m_testing) {
				std::cout << "done!" << std::endl;
			}
		}
	}

	if (input->getKeyPressed(key_f1)) {
		startTest(false);
	}

	if (input->getKeyPressed(key_f2)) {
		startTest(true);
	}

	if (input->getKeyPressed(key_enter)) {
		saveResults();
	}

	if (input->getKeyPressed(key_comma)) {
		addLights(1);
	}

	if (input->getKeyPressed(key_period)) {
		addLights(10);
	}

	if (input->getKeyPressed(key_m)) {
		setLightsEnabled(!m_lightsEnabled);
	}

	if (input->getKeyPressed(key_h)) {
		setLightsMoving(!m_lightsMoving);
	}
}

void LightingTest::addLights(unsigned int num)
{
	Scene* scene = getEntity()->getParentScene();
	if (!scene) return;

	for (unsigned int i = 0; i < num; ++i) {
		auto entity = std::make_unique<Entity>();

		auto light = entity->addComponent<Light>();
		light->setType(Light::type_point);

		// generate color from random hue and saturation (but always full brightness)
		glm::vec4 color(glm::rgbColor(glm::vec3(
			random::uniform_float(360.0f),
			random::uniform_float(m_minSaturation, m_maxSaturation),
			1.0f)), 1.0f);

		light->setColor(color);

		float intensity = random::uniform_float(m_minIntensity, m_maxIntensity);
		light->setIntensity(intensity);

		float range = glm::sqrt(intensity) * m_rangeFactor; // make range proportional to intensity (multiplied by some factor) so it doesn't look weird
		light->setRange(range);

		auto rm = entity->addComponent<RandomMovement>();
		rm->setArea(m_lightArea);
		rm->setMinMaxSpeed(m_minSpeed, m_maxSpeed);

		auto e = entity.get();
		m_lights.push_back(e);
		scene->addEntity(std::move(entity));

		e->setActive(m_lightsEnabled);
		rm->setEnabled(m_lightsMoving);
	}

	std::cout << "added " << num << " lights, " << m_lights.size() << " total" << std::endl;
}

void LightingTest::setLightsEnabled(bool val)
{
	if (val != m_lightsEnabled) {
		m_lightsEnabled = val;

		for (Entity* e : m_lights) {
			e->setActive(m_lightsEnabled);
		}
	}
}

void LightingTest::setLightsMoving(bool val)
{
	if (val != m_lightsMoving) {
		m_lightsMoving = val;

		for (Entity* e : m_lights) {
			e->getComponent<RandomMovement>()->setEnabled(m_lightsMoving);
		}
	}
}

void LightingTest::startTest(bool lights)
{
	m_testTimer = Engine::instance()->getTime() + m_testDuration;
	m_testing = true;
	m_testLights = lights;
	m_testLight = 0;
	m_testSamples = 0;
	m_accFrameTime = 0.0;

	if (lights) {
		setLightsEnabled(false);
	}

	m_ftResults.clear();
	m_fpsResults.clear();
	m_lightsPerObj.clear();
}

void LightingTest::saveResults()
{
	{
		std::ofstream file("test.dat");

		file << "lights frameTime fps" << std::endl;

		if (file) {
			for (unsigned int i = 0; i < m_ftResults.size(); ++i) {
				file << (i + 1) << " " << m_ftResults[i] << " " << m_fpsResults[i] << std::endl;
			}

			std::cout << "saved test results" << std::endl;
		}
	}
	{
		std::ofstream file("test2.dat");

		file << "lights lpo" << std::endl;

		if (file) {
			for (unsigned int i = 0; i < m_lightsPerObj.size(); ++i) {
				file << (i + 1) << " " << m_lightsPerObj[i] << std::endl;
			}
		}
	}
}
