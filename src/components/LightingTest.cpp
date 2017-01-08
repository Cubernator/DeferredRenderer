#include "components/LightingTest.hpp"
#include "components/LightRandomiser.hpp"
#include "util/component_registry.hpp"
#include "graphics/RenderEngine.hpp"
#include "graphics/Light.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "core/Input.hpp"

REGISTER_COMPONENT_CLASS(LightingTest, "lightingTest");

json_interpreter<LightingTest> LightingTest::s_properties{
	{ "lightCount", &LightingTest::m_lightCount },
	{ "lightArea", &LightingTest::m_lightArea }
};


LightingTest::LightingTest(Entity* parent) : Component(parent), m_lightsEnabled(true), m_lightCount(0) { }

void LightingTest::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void LightingTest::start_impl()
{
	Scene* scene = getEntity()->getParentScene();
	if (!scene) return;

	for (unsigned int i = 0; i < m_lightCount; ++i) {
		auto entity = std::make_unique<Entity>();

		entity->addComponent<Light>();
		auto lr = entity->addComponent<LightRandomizer>();
		lr->setArea(m_lightArea);

		m_lights.push_back(entity.get());
		scene->addEntity(std::move(entity));
	}

	setLightsEnabled(false);
}

void LightingTest::update_impl()
{
	Engine* engine = Engine::instance();
	RenderEngine* graphics = RenderEngine::instance();
	Input* input = Input::instance();

	if (input->getKeyPressed(GLFW_KEY_ESCAPE)) {
		engine->stop();
	}

	if (input->getKeyPressed(GLFW_KEY_P)) {
		bool d = !graphics->isDeferredEnabled();
		graphics->setDeferredEnabled(d);
		std::cout << "Rendering path: " << (d ? "Deferred" : "Forward") << std::endl;
	}

	if (input->getKeyPressed(GLFW_KEY_O)) {
		bool vfc = !graphics->isVFCEnabled();
		graphics->setVFCEnabled(vfc);
		std::cout << "View frustum culling " << (vfc ? "Enabled" : "Disabled") << std::endl;
	}

	if (input->getKeyPressed(GLFW_KEY_G)) {
		int om = int(graphics->getOutputMode());
		om = (om + 1) % RenderEngine::output_mode_max;
		graphics->setOutputMode(static_cast<RenderEngine::output_mode>(om));
		std::cout << "Debug output: " << getOutputName(om) << std::endl;
	}

	if (input->getKeyPressed(GLFW_KEY_N)) {
		setLightsEnabled(!m_lightsEnabled);
	}
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

std::string LightingTest::getOutputName(int o)
{
	auto om = static_cast<RenderEngine::output_mode>(o);
	switch (om) {
	case RenderEngine::output_default:
		return "Default";
	case RenderEngine::output_diffuse:
		return "Diffuse Color";
	case RenderEngine::output_specular:
		return "Specular Color";
	case RenderEngine::output_smoothness:
		return "Smoothness";
	case RenderEngine::output_normal:
		return "Normals";
	default:
		return "???";
	}
}