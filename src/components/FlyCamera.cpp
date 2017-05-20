#include "FlyCamera.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/RenderEngine.hpp"
#include "util/component_registry.hpp"
#include "input/Input.hpp"

REGISTER_COMPONENT_CLASS(FlyCamera, "flyCamera");

json_interpreter<FlyCamera> FlyCamera::s_properties({
	{ "normalSpeed", &FlyCamera::m_normalSpeed },
	{ "fastSpeed", &FlyCamera::m_fastSpeed },
	{ "sensitivityX", &FlyCamera::m_sensitivityX },
	{ "sensitivityY", &FlyCamera::m_sensitivityY },
	{ "angleX", &FlyCamera::setAngleXDeg },
	{ "angleY", &FlyCamera::setAngleYDeg }
});

FlyCamera::FlyCamera(Entity* parent) : Component(parent), m_normalSpeed(1.0f), m_fastSpeed(5.0f),
	m_sensitivityX(1.0f), m_sensitivityY(1.0f),
	m_angleX(0.0f), m_angleY(0.0f), m_transform(nullptr) { }

void FlyCamera::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void FlyCamera::start_impl()
{
	m_transform = getEntity()->getTransform();
	updateRotation();
}

void FlyCamera::update_impl()
{
	Engine* engine = Engine::instance();
	Input* input = Input::instance();

	bool locked = input->isCursorLocked();
	if (input->getMouseButtonPressed(mbutton_left)) {
		locked = !locked;
		input->setCursorLocked(locked);
	}

	if (locked) {
		glm::vec2 delta = input->getCursorDelta();

		m_angleX -= delta.y * m_sensitivityY * 0.017f;
		m_angleY -= delta.x * m_sensitivityX * 0.017f;

		m_angleX = glm::clamp(m_angleX, -glm::half_pi<float>(), glm::half_pi<float>());
		m_angleY = glm::wrapAngle(m_angleY);

		auto rot = updateRotation();

		float x = 0.0f;
		if (input->getKey(key_a))
			x -= 1.0f;
		if (input->getKey(key_d))
			x += 1.0f;

		float z = 0.0f;
		if (input->getKey(key_w))
			z -= 1.0f;
		if (input->getKey(key_s))
			z += 1.0f;

		float speed = input->getKey(key_left_shift) ? m_fastSpeed : m_normalSpeed;

		glm::vec3 dir = rot * glm::vec3(x, 0.0f, z);

		glm::vec3 pos = m_transform->getPosition();
		pos += dir * speed * float(engine->getFrameTime());
		m_transform->setPosition(pos);
	}

	if (input->getKeyPressed(key_z)) {
		auto pos = m_transform->getPosition();
		std::cout << "pos: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
		std::cout << "angle: (" << glm::degrees(m_angleX) << ", " << glm::degrees(m_angleY) << ")" << std::endl;
	}
}

glm::quat FlyCamera::updateRotation()
{
	glm::quat rotX = glm::angleAxis(m_angleX, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis(m_angleY, glm::vec3(0, 1, 0));
	glm::quat rot = rotY * rotX;
	m_transform->setRotation(rot);
	return rot;
}

void FlyCamera::setAngleXDeg(float degrees)
{
	m_angleX = glm::radians(degrees);
}

void FlyCamera::setAngleYDeg(float degrees)
{
	m_angleY = glm::radians(degrees);
}

