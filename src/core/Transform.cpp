#include "core/Transform.hpp"
#include "core/Entity.hpp"

#include "util/json_utils.hpp"

REGISTER_COMPONENT_CLASS_DEF(Transform, "transform");

json_interpreter<Transform> Transform::s_properties({
	{ "position", &Transform::extractPosition },
	{ "rotation", &Transform::extractRotation },
	{ "scale", &Transform::extractScale }
});

Transform::Transform(Entity* parent) : Component(parent), m_scale(1.0f), m_dirty(true) { }

const glm::mat4& Transform::getMatrix()
{
	if (m_dirty) {
		m_cachedMatrix = glm::toMat4(m_rotation) * glm::scale(m_scale);
		m_cachedMatrix = glm::translate(m_cachedMatrix, m_position);
		m_dirty = false;
	}

	return m_cachedMatrix;
}

glm::mat4 Transform::getRigidMatrix() const
{
	return glm::translate(glm::toMat4(m_rotation), m_position);
}

glm::mat4 Transform::getInverseRigidMatrix() const
{
	return glm::translate(glm::toMat4(glm::inverse(m_rotation)), -m_position);
}

void Transform::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	s_properties.interpret_property(name, this, json);
}

void Transform::extractPosition(const nlohmann::json& json)
{
	setPosition(json_get<glm::vec3>(json));
}

void Transform::extractRotation(const nlohmann::json& json)
{
	setRotation(json_get<glm::quat>(json));
}

void Transform::extractScale(const nlohmann::json& json)
{
	setScale(json_get<glm::vec3>(json));
}

