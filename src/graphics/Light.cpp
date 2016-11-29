#include "graphics/Light.hpp"
#include "graphics/Renderer.hpp"
#include "util/json_utils.hpp"
#include "util/intersection_tests.hpp"
#include "util/component_registry.hpp"
#include "core/Transform.hpp"
#include "core/Entity.hpp"

REGISTER_COMPONENT_CLASS(Light, "light");

json_interpreter<Light> Light::s_properties({
	{ "lightType", {&Light::setType, &Light::s_types} },
	{ "color", &Light::setColor },
	{ "intensity", &Light::setIntensity },
	{ "range", &Light::setRange },
	{ "priority", &Light::setPriority }
});

keyword_helper<Light::type> Light::s_types({
	{ "directional", Light::type_directional },
	{ "point", Light::type_point },
	{ "spot", Light::type_spot }
});

Light::Light(Entity* parent) : Component(parent), m_type(type_directional), m_color(1.0f), m_intensity(1.0f), m_range(10.0f), m_priority(0) { }

bool Light::isVisible() const
{
	return (m_range > 0.0f) && (m_intensity > 0.0f);
}

glm::vec4 Light::getShaderProp() const
{
	const Transform* trans = getEntity()->getTransform();
	if (m_type == type_directional) {
		glm::vec3 dir(0.0f, 0.0f, 1.0f);
		dir = trans->getRotation() * dir;
		return glm::vec4(dir, 0.0f);
	} else {
		return glm::vec4(trans->getPosition(), 1.0f);
	}
}

glm::vec4 Light::getPremultipliedColor() const
{
	return m_color * m_intensity;
}

bool Light::checkIntersection(const Renderer* renderer) const
{
	if (m_type == type_directional) return true;

	const Transform* myTrans = getEntity()->getTransform();
	const Transform* rTrans = renderer->getEntity()->getTransform();

	// get light position relative to renderer's coordinate system (without scale)
	glm::vec4 mp(myTrans->getPosition(), 1.0f);
	mp = rTrans->getInverseRigidMatrix() * mp;
	glm::vec3 myPos(mp);

	// apply renderer's scale directly to bounds
	aabb rBounds = renderer->getBounds();
	rBounds.min *= rTrans->getScale();
	rBounds.max *= rTrans->getScale();

	// intersect aabb (renderer's bounds) with lights's sphere of influence
	return intersect_aabb_sphere(rBounds, myPos, m_range);
}

void Light::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

