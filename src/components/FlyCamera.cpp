#include "FlyCamera.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/RenderEngine.hpp"
#include "util/component_registry.hpp"
#include "core/Input.hpp"

REGISTER_COMPONENT_CLASS(FlyCamera, "flyCamera");

json_interpreter<FlyCamera> FlyCamera::s_properties({
	{ "normalSpeed", &FlyCamera::m_normalSpeed },
	{ "fastSpeed", &FlyCamera::m_fastSpeed },
	{ "sensitivityX", &FlyCamera::m_sensitivityX },
	{ "sensitivityY", &FlyCamera::m_sensitivityY }
});

FlyCamera::FlyCamera(Entity* parent) : Component(parent), m_normalSpeed(1.0f), m_sensitivityX(1.0f), m_sensitivityY(1.0f), m_angleX(0.0f), m_angleY(0.0f) { }

void FlyCamera::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void FlyCamera::update_impl()
{
	Transform* trans = getEntity()->getTransform();
	Engine* engine = Engine::instance();
	Input* input = Input::instance();

	bool locked = input->isCursorLocked();
	if (input->getMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
		locked = !locked;
		input->setCursorLocked(locked);
	}

	if (locked) {
		float dt = float(engine->getDeltaTime());

		glm::vec2 delta = input->getCursorDelta();

		m_angleX -= delta.y * m_sensitivityY * dt;
		m_angleY -= delta.x * m_sensitivityX * dt;

		m_angleX = glm::clamp(m_angleX, -glm::half_pi<float>(), glm::half_pi<float>());
		m_angleY = glm::wrapAngle(m_angleY);

		glm::quat rotX = glm::angleAxis(m_angleX, glm::vec3(1, 0, 0));
		glm::quat rotY = glm::angleAxis(m_angleY, glm::vec3(0, 1, 0));
		glm::quat rot = rotY * rotX;

		float x = 0.0f;
		if (input->getKey(GLFW_KEY_A))
			x -= 1.0f;
		if (input->getKey(GLFW_KEY_D))
			x += 1.0f;

		float z = 0.0f;
		if (input->getKey(GLFW_KEY_W))
			z -= 1.0f;
		if (input->getKey(GLFW_KEY_S))
			z += 1.0f;

		float speed = input->getKey(GLFW_KEY_LEFT_SHIFT) ? m_fastSpeed : m_normalSpeed;

		glm::vec3 dir = rot * glm::vec3(x, 0.0f, z);

		glm::vec3 pos = trans->getPosition();
		pos += dir * speed * dt;
		trans->setPosition(pos);
		trans->setRotation(rot);
	}
}

