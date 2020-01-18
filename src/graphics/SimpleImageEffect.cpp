#include "SimpleImageEffect.hpp"
#include "RenderEngine.hpp"
#include "Material.hpp"
#include "Effect.hpp"
#include "texture/RenderTexture.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "core/component_registry.hpp"
#include "content/pooled.hpp"
#include "scripting/class_registry.hpp"

namespace hexeract
{
	namespace graphics
	{
		REGISTER_COMPONENT_CLASS(SimpleImageEffect);

		json_interpreter<SimpleImageEffect> SimpleImageEffect::s_properties({
			{ "material", &SimpleImageEffect::extractMaterial },
			{ "outputLinear", &SimpleImageEffect::setOutputLinear }
		});

		SimpleImageEffect::SimpleImageEffect(Entity* parent) : ImageEffect(parent), m_material(nullptr), m_outputLinear(true) { }

		void SimpleImageEffect::apply(const Texture2D* input, const RenderTexture* output)
		{
			if (m_material) {
				Effect* effect = m_material->effect();
				if (effect) {
					auto graphics = RenderEngine::instance();
					graphics->setConvertToSRGB(m_outputLinear);

					auto passCount = effect->passCount();

					const RenderTexture *curOutput = output, *curInput = graphics->getAuxRenderTexture();
					if (passCount % 2 == 0) {
						std::swap(curInput, curOutput);
					}

					for (std::size_t i = 0; i < passCount; ++i) {
						const Texture2D* blitinput = (i == 0) ? input : curInput;
						graphics->blit(blitinput, curOutput, m_material, i);
						std::swap(curInput, curOutput);
					}
				}
			}
		}

		bool SimpleImageEffect::isGood() const
		{
			return m_material && m_material->effect();
		}

		void SimpleImageEffect::apply_json_impl(const nlohmann::json& json)
		{
			Component::apply_json_impl(json);
			s_properties.interpret_all(this, json);
		}

		void SimpleImageEffect::extractMaterial(const nlohmann::json& json)
		{
			m_material = content::get_pooled_json<Material>(json);
		}

		SCRIPTING_REGISTER_DERIVED_CLASS(SimpleImageEffect, ImageEffect);
	}
}
