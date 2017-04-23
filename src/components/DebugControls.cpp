#include "DebugControls.hpp"
#include "util/component_registry.hpp"
#include "core/Entity.hpp"
#include "core/Engine.hpp"
#include "graphics/RenderEngine.hpp"
#include "graphics/Camera.hpp"
#include "graphics/ImageEffect.hpp"
#include "input/Input.hpp"

REGISTER_COMPONENT_CLASS(DebugControls, "debugControls");

DebugControls::DebugControls(Entity* parent) : Component(parent), m_imgEffectsEnabled(true) { }

void DebugControls::start_impl()
{
	auto cam = Camera::main();
	if (cam) {
		m_imgEffects = cam->getEntity()->getComponents<ImageEffect>();
	}
}

void DebugControls::update_impl()
{
	Engine* engine = Engine::instance();
	RenderEngine* graphics = RenderEngine::instance();
	Input* input = Input::instance();

	if (input->getKeyPressed(key_escape)) {
		engine->stop();
	}

	if (input->getKeyPressed(key_p)) {
		bool d = !(graphics->isDeferredEnabled());
		graphics->setDeferredEnabled(d);
		std::cout << "Rendering path: " << (d ? "Deferred" : "Forward") << std::endl;
	}

	if (input->getKeyPressed(key_o)) {
		bool vfc = !(graphics->isVFCEnabled());
		graphics->setVFCEnabled(vfc);
		std::cout << "View frustum culling " << (vfc ? "Enabled" : "Disabled") << std::endl;
	}

	if (input->getKeyPressed(key_g)) {
		int om = int(graphics->getOutputMode());
		om = (om + 1) % RenderEngine::output_mode_max;
		graphics->setOutputMode(static_cast<RenderEngine::output_mode>(om));
		std::cout << "Debug output: " << getOutputName(om) << std::endl;
	}

	if (input->getKeyPressed(key_i)) {
		setImageEffectsEnabled(!m_imgEffectsEnabled);
		std::cout << "Image Effects " << (m_imgEffectsEnabled ? "Enabled" : "Disabled") << std::endl;
	}

	if (input->getKey(key_u)) {
		std::cout << "\r" << "FPS: " << engine->getFPS();
	}

	if (input->getKeyPressed(key_t)) {
		std::cout << "Triangle count: " << graphics->getTriangleCount() << std::endl;
	}

	if (input->getKey(key_r)) {
		engine->loadFirstScene();
	}
}

void DebugControls::setImageEffectsEnabled(bool val)
{
	if (val != m_imgEffectsEnabled) {
		m_imgEffectsEnabled = val;

		for (ImageEffect* e : m_imgEffects) {
			e->setEnabled(m_imgEffectsEnabled);
		}
	}
}

std::string DebugControls::getOutputName(int o)
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
	case RenderEngine::output_depth:
		return "Depth";
	default:
		return "???";
	}
}