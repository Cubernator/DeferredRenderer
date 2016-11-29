#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "util/component_registry.hpp"
#include "graphics/Camera.hpp"

REGISTER_COMPONENT_CLASS(Camera, "camera");

Camera * Camera::s_mainCamera(nullptr);

json_interpreter<Camera> Camera::s_properties({
	{ "fov",		&Camera::setFov },
	{ "nearPlane",	&Camera::setNearPlane },
	{ "farPlane",	&Camera::setFarPlane }
});

Camera::Camera(Entity* parent) : Component(parent)
{
	if (!main()) {
		setAsMain();
	}
}

Camera::~Camera()
{
	if (main() == this) {
		setMain(nullptr);
	}
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const
{
	return glm::perspectiveFov(glm::radians(m_fov), width, height, m_nearPlane, m_farPlane);
}

void Camera::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

