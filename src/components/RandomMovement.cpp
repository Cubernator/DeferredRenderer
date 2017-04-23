#include "RandomMovement.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/Light.hpp"
#include "input/Input.hpp"
#include "util/component_registry.hpp"
#include "util/random.hpp"

REGISTER_COMPONENT_CLASS(RandomMovement, "randomMovement");

RandomMovement::RandomMovement(Entity* parent) : Component(parent), m_minSpeed(0.1f), m_maxSpeed(20.0f) { }

void RandomMovement::start_impl()
{
	m_transform = getEntity()->getTransform();

	glm::vec3 pos = random::uniform_vec3(m_area.min, m_area.max);
	m_transform->setPosition(pos);

	m_velocity = random::uniform_direction() * random::uniform_float(m_minSpeed, m_maxSpeed);
}

void RandomMovement::update_impl()
{
	glm::vec3 pos = m_transform->getPosition();

	// apply velocity
	pos += m_velocity * Engine::instance()->getFrameTime();

	// bounce off the walls of an imaginary box
	for (unsigned int i = 0; i < 3; ++i) {
		if ((pos[i] < m_area.min[i]) || (pos[i] > m_area.max[i]))
			m_velocity[i] = -m_velocity[i];
	}

	m_transform->setPosition(pos);
}
