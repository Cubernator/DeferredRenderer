#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "core/Component.hpp"
#include "util/bounds.hpp"
#include "glm.hpp"

class Camera : public Component
{
public:
	explicit Camera(Entity* parent);
	~Camera();

	void setAsMain() { setMain(this); }

	float fov() const { return m_fov; }
	float nearPlane() const { return m_nearPlane; }
	float farPlane() const { return m_farPlane; }

	void setFov(float val) { m_fov = val; }
	void setNearPlane(float val) { m_nearPlane = val; }
	void setFarPlane(float val) { m_farPlane = val; }

	glm::mat4 getProjectionMatrix(float width, float height) const;

	static Camera* main() { return s_mainCamera; }
	static void setMain(Camera *cam) { s_mainCamera = cam; }

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void apply_json_impl(const nlohmann::json& json) override;

private:
	float m_fov;
	float m_nearPlane;
	float m_farPlane;

	static Camera *s_mainCamera;

	static json_interpreter<Camera> s_properties;
};

#endif // CAMERA_HPP