#include "Transform.hpp"
#include "Entity.hpp"
#include "core/component_registry.hpp"
#include "util/json_utils.hpp"
#include "scripting/class_registry.hpp"

REGISTER_COMPONENT_CLASS(Transform);

json_interpreter<Transform> Transform::s_properties({
	{ "position", &Transform::setPosition },
	{ "rotation", &Transform::setRotation },
	{ "scale", &Transform::setScale }
});

Transform::Transform(Entity* parent) : Component(parent), m_scale(1.0f), m_dirty(true) { }

const glm::mat4& Transform::getMatrix() const
{
	if (m_dirty) {
		m_cachedMatrix = glm::translate(m_position) * glm::toMat4(m_rotation) * glm::scale(m_scale);
		m_dirty = false;
	}

	return m_cachedMatrix;
}

glm::mat4 Transform::getInverseMatrix() const
{
	return glm::scale(getInverseRigidMatrix(), { 1.f / m_scale.x, 1.f / m_scale.y, 1.f / m_scale.z });
}



glm::mat4 Transform::getRigidMatrix() const
{
	return glm::translate(m_position) * glm::toMat4(m_rotation);
}

glm::mat4 Transform::getInverseRigidMatrix() const
{
	return glm::translate(glm::toMat4(glm::inverse(m_rotation)), -m_position);
}

void Transform::apply_json_impl(const nlohmann::json& json)
{
	Component::apply_json_impl(json);
	s_properties.interpret_all(this, json);
}

SCRIPTING_REGISTER_DERIVED_CLASS(Transform, Component)

SCRIPTING_AUTO_METHOD(Transform, position)
SCRIPTING_AUTO_METHOD(Transform, rotation)
SCRIPTING_AUTO_METHOD(Transform, scale)
SCRIPTING_AUTO_METHOD(Transform, setPosition)
SCRIPTING_AUTO_METHOD(Transform, setRotation)
SCRIPTING_AUTO_METHOD(Transform, setScale)
