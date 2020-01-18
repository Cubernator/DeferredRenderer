#include "Camera.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "core/component_registry.hpp"
#include "scripting/class_registry.hpp"

namespace hexeract
{
	namespace graphics
	{
		REGISTER_COMPONENT_CLASS(Camera);

		Camera * Camera::s_mainCamera(nullptr);

		namespace
		{
			json_interpreter<Camera> g_properties({
				{ "fov",		&Camera::setFov },
				{ "nearPlane",	&Camera::setNearPlane },
				{ "farPlane",	&Camera::setFarPlane }
			});
		}

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

		void Camera::apply_json_impl(const nlohmann::json& json)
		{
			Component::apply_json_impl(json);
			g_properties.interpret_all(this, json);
		}

		SCRIPTING_REGISTER_DERIVED_CLASS(Camera, Component);

		SCRIPTING_AUTO_METHOD(Camera, setAsMain);

		SCRIPTING_AUTO_METHOD(Camera, fov);
		SCRIPTING_AUTO_METHOD(Camera, nearPlane);
		SCRIPTING_AUTO_METHOD(Camera, farPlane);

		SCRIPTING_AUTO_METHOD(Camera, setFov);
		SCRIPTING_AUTO_METHOD(Camera, setNearPlane);
		SCRIPTING_AUTO_METHOD(Camera, setFarPlane);

		SCRIPTING_DEFINE_METHOD(Camera, main)
		{
			scripting::push_object(L, Camera::main());
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Camera, setMain)
		{
			auto cam = scripting::check_arg<Camera*>(L, 1);
			Camera::setMain(cam);
			return 0;
		}
	}
}