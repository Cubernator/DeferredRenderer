#include "components/LightRandomiser.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/Light.hpp"
#include "util/component_registry.hpp"
#include "util/random.hpp"

REGISTER_COMPONENT_CLASS(LightRandomizer, "lightRandomizer");

LightRandomizer::LightRandomizer(Entity* parent) : Component(parent) { }

void LightRandomizer::start_impl()
{
	m_transform = getEntity()->getTransform();

	glm::vec3 pos = random::uniform_vec3(m_area.min, m_area.max);
	m_transform->setPosition(pos);

	m_light = getEntity()->getComponent<Light>();

	// Please excuse all the hardcoded values... I'm sorry :(

	if (m_light) {
		m_light->setType(Light::type_point);

		// generate color from random hue and saturation (but always full brightness)
		glm::vec4 color(glm::rgbColor(glm::vec3(
			random::uniform_float(360.0f),
			random::uniform_float(.6f, 1.f),
			1.0f)), 1.0f);

		m_light->setColor(color);

		float intensity = random::uniform_float(1.f, 8.f);
		m_light->setIntensity(intensity);

		float range = glm::sqrt(intensity) * 4.0f; // make range proportional to intensity (multiplied by some factor) so it doesn't look weird
		m_light->setRange(range);
	}

	m_velocity = random::uniform_direction() * random::uniform_float(0.1f, 20.0f);
}

void LightRandomizer::update_impl()
{
	glm::vec3 pos = m_transform->getPosition();

	// apply velocity
	pos += m_velocity * Engine::instance()->getDeltaTime();

	// bounce off the walls of an imaginary box
	for (unsigned int i = 0; i < 3; ++i) {
		if ((pos[i] < m_area.min[i]) || (pos[i] > m_area.max[i]))
			m_velocity[i] = -m_velocity[i];
	}

	m_transform->setPosition(pos);
}
