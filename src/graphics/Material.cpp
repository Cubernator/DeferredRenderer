#include "graphics/Material.hpp"
#include "graphics/Effect.hpp"
#include "util/type_registry.hpp"
#include "core/Content.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Material, "material");

Material::Material() : m_effect(nullptr) { }

void Material::setEffect(Effect* effect)
{
	m_effect = effect;

	if (m_effect) {
		for (auto it = m_effect->begin_properties(); it != m_effect->end_properties(); ++it) {
			auto pit = m_properties.find(it->id);
			if (pit != m_properties.end()) {
				if (!is_same_type(*it, *pit)) {
					m_properties.replace(pit, *it);
				}
			} else {
				m_properties.insert(*it);
			}
		}
	}
}

const shader_property* Material::getProperty(uniform_id id) const
{
	auto it = m_properties.find(id);
	if (it != m_properties.end()) {
		return &(*it);
	}
	return nullptr;
}

void Material::apply_json_impl(const nlohmann::json& json)
{
	auto eit = json.find("effect");
	if (eit != json.end()) {
		Effect* effect = Content::instance()->getPooledFromJson<Effect>(*eit);
		setEffect(effect);
	}

	auto pit = json.find("properties");
	if (pit != json.end() && pit->is_object()) {
		auto& pj = *pit;
		for (auto it = pj.begin(); it != pj.end(); ++it) {
			setPropFromJson(it.key(), it.value());
		}
	}
}

void Material::setPropFromJson(const std::string& name, const nlohmann::json& json)
{
	auto it = m_properties.find(uniform_name_to_id(name));
	if (it != m_properties.end()) {
		m_properties.modify(it, [&json](shader_property& prop) {
			prop.assign_json(json);
		});
	}
}
