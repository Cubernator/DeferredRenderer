#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "util/component_registry.hpp"
#include "SimpleImageEffect.hpp"
#include "RenderEngine.hpp"
#include "Material.hpp"
#include "Effect.hpp"
#include "texture/RenderTexture.hpp"
#include "content/Content.hpp"

REGISTER_COMPONENT_CLASS(SimpleImageEffect, "simpleImageEffect");

json_interpreter<SimpleImageEffect> SimpleImageEffect::s_properties({
	{ "material", &SimpleImageEffect::extractMaterial },
	{ "outputLinear", &SimpleImageEffect::m_outputLinear }
});

SimpleImageEffect::SimpleImageEffect(Entity* parent) : ImageEffect(parent), m_material(nullptr), m_outputLinear(true) { }

void SimpleImageEffect::apply(const Texture2D* input, const RenderTexture* output)
{
	if (m_material) {
		Effect* effect = m_material->getEffect();
		if (effect) {
			auto graphics = RenderEngine::instance();
			graphics->setConvertToSRGB(m_outputLinear);

			unsigned int passCount = effect->passCount();

			const RenderTexture *curOutput = output, *curInput = graphics->getAuxRenderTexture();
			if (passCount % 2 == 0) {
				std::swap(curInput, curOutput);
			}
			
			for (unsigned int i = 0; i < passCount; ++i) {
				const Texture2D* blitinput = (i == 0) ? input : curInput;
				graphics->blit(blitinput, curOutput, m_material, i);
				std::swap(curInput, curOutput);
			}
		}
	}
}

void SimpleImageEffect::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void SimpleImageEffect::extractMaterial(const nlohmann::json& json)
{
	m_material = Content::instance()->getPooledFromJson<Material>(json);
}
