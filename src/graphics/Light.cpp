#include "Light.hpp"
#include "Renderer.hpp"
#include "util/json_utils.hpp"
#include "core/component_registry.hpp"
#include "core/Transform.hpp"
#include "core/Entity.hpp"
#include "scripting/class_registry.hpp"

namespace hexeract
{
	namespace graphics
	{
		REGISTER_COMPONENT_CLASS(Light);

		namespace
		{
			keyword_helper<Light::light_type> g_types({
				{ "directional",	Light::type_directional },
				{ "point",			Light::type_point },
				{ "spot",			Light::type_spot }
			});

			json_interpreter<Light> g_properties({
				{ "lightType",		{ &Light::setType, &g_types} },
				{ "color",			&Light::setColor },
				{ "intensity",		&Light::setIntensity },
				{ "range",			&Light::setRange },
				{ "spotAngle",		&Light::setSpotAngle },
				{ "spotFalloff",	&Light::setSpotFalloff },
				{ "priority",		&Light::setPriority }
			});
		}

		Light::Light(Entity* parent) : Component(parent), m_type(type_directional), m_color(1.f), m_intensity(1.f), m_range(10.f), m_spotAngle(15.f), m_spotFalloff(0.5f), m_priority(0) { }

		bool Light::isVisible() const
		{
			return (m_range > 0.0f) && (m_intensity > 0.0f);
		}

		void Light::getUniforms(glm::vec4& color, glm::vec4& dir, glm::vec4& atten, glm::vec4& spot) const
		{
			color = m_color * m_intensity;

			const Transform* trans = entity()->transform();
			glm::vec3 f(0.0f, 0.0f, -1.0f);
			f = trans->rotation() * f;

			if (m_type == type_directional) {
				dir = { f, 0.0f };
			} else {
				dir = { trans->position(), 1.0f };
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
			g_properties.interpret_all(this, json);
		}


		SCRIPTING_REGISTER_DERIVED_CLASS(Light, Component);

		SCRIPTING_AUTO_METHOD(Light, color);
		SCRIPTING_AUTO_METHOD(Light, intensity);
		SCRIPTING_AUTO_METHOD(Light, range);
		SCRIPTING_AUTO_METHOD(Light, spotAngle);
		SCRIPTING_AUTO_METHOD(Light, spotFalloff);
		SCRIPTING_AUTO_METHOD(Light, priority);

		SCRIPTING_AUTO_METHOD(Light, setColor);
		SCRIPTING_AUTO_METHOD(Light, setIntensity);
		SCRIPTING_AUTO_METHOD(Light, setRange);
		SCRIPTING_AUTO_METHOD(Light, setSpotAngle);
		SCRIPTING_AUTO_METHOD(Light, setSpotFalloff);
		SCRIPTING_AUTO_METHOD(Light, setPriority);

		SCRIPTING_AUTO_METHOD(Light, isVisible);

		SCRIPTING_AUTO_METHOD(Light, type);
		SCRIPTING_DEFINE_METHOD(Light, setType)
		{
			auto self = scripting::check_self<Light>(L);
			// TODO: expose enum to lua
			auto tn = scripting::check_arg<std::string>(L, 2);
			Light::light_type val;
			if (g_types.get(tn, val)) {
				self->setType(val);
			} else {
				luaL_argerror(L, 2, "unknown light type");
			}
			return 0;
		}
	}
}
