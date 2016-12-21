#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "glm.hpp"
#include "Component.hpp"
#include "util/json_interpreter.hpp"

class Transform : public Component
{
public:
	Transform(Entity* parent);

	const glm::vec3& getPosition() const { return m_position; }
	const glm::quat& getRotation() const { return m_rotation; }
	const glm::vec3& getScale() const { return m_scale; }

	void setPosition(const glm::vec3& position) { m_position = position; m_dirty = true; }
	void setRotation(const glm::quat& rotation) { m_rotation = rotation; m_dirty = true; }
	void setScale(const glm::vec3& scale) { m_scale = scale; m_dirty = true; }

	const glm::mat4& getMatrix() const;
	glm::mat4 getRigidMatrix() const;
	glm::mat4 getInverseRigidMatrix() const;

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;

	mutable glm::mat4 m_cachedMatrix;
	mutable bool m_dirty;

	static json_interpreter<Transform> s_properties;
};

#endif // TRANSFORM_HPP