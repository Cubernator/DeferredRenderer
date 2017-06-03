#include "Light.hpp"
#include "Renderer.hpp"
#include "util/json_utils.hpp"
#include "util/component_registry.hpp"
#include "core/Transform.hpp"
#include "core/Entity.hpp"

REGISTER_COMPONENT_CLASS(Light, "light");

json_interpreter<Light> Light::s_properties({
	{ "lightType",		{ &Light::setType, &Light::s_types} },
	{ "color",			&Light::setColor },
	{ "intensity",		&Light::setIntensity },
	{ "range",			&Light::setRange },
	{ "spotAngle",		&Light::setSpotAngle },
	{ "spotFalloff",	&Light::setSpotFalloff },
	{ "priority",		&Light::setPriority }
});

keyword_helper<Light::type> Light::s_types({
	{ "directional",	Light::type_directional },
	{ "point",			Light::type_point },
	{ "spot",			Light::type_spot }
});

Light::Light(Entity* parent) : Component(parent), m_type(type_directional), m_color(1.f), m_intensity(1.f), m_range(10.f), m_spotAngle(15.f), m_spotFalloff(0.5f), m_priority(0) { }

bool Light::isVisible() const
{
	return (m_range > 0.0f) && (m_intensity > 0.0f);
}

void Light::getUniforms(glm::vec4& color, glm::vec4& dir, glm::vec4& atten, glm::vec4& spot) const
{
	color = m_color * m_intensity;

	const Transform* trans = getEntity()->getTransform();
	glm::vec3 f(0.0f, 0.0f, -1.0f);
	f = trans->getRotation() * f;

	if (m_type == type_directional) {
		dir = { f, 0.0f };
	} else {
		dir = { trans->getPosition(), 1.0f };
		atten.x = m_range * m_range;
	}

	if (m_type == type_spot) {
		float ca = cosf(glm::radians(m_spotAngle));
		float cf = cosf(glm::radians(m_spotAngle * m_spotFalloff));
		spot = { f, ca };
		atten.y = cf;
		atten.z = 1.0f / (ca - cf);
	} else {
		spot = { 0.f, 0.f, 0.f, -1.f };
	}
}

void Light::apply_json_impl(const nlohmann::json& json)
{
	Component::apply_json_impl(json);
	s_properties.interpret_all(this, json);
}

