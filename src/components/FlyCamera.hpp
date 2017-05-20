#ifndef FLYCAMERA_HPP
#define FLYCAMERA_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"

class Transform;

class FlyCamera : public Component
{
public:
	explicit FlyCamera(Entity* parent);

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json);
	virtual void start_impl() override;
	virtual void update_impl() override;

private:
	float m_normalSpeed, m_fastSpeed;
	float m_sensitivityX, m_sensitivityY;
	float m_angleX, m_angleY;

	Transform* m_transform;

	glm::quat updateRotation();

	// cppcheck-suppress unusedPrivateFunction
	void setAngleXDeg(float degrees);
	// cppcheck-suppress unusedPrivateFunction
	void setAngleYDeg(float degrees);

	static json_interpreter<FlyCamera> s_properties;

	friend class json_interpreter<FlyCamera>;
};

#endif // FLYCAMERA_HPP