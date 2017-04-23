#include "Renderer.hpp"
#include "Material.hpp"
#include "content/Content.hpp"

json_interpreter<Renderer> Renderer::s_properties({
	{ "materials", &Renderer::extractMaterials }
});


Renderer::Renderer(Entity* parent) : Component(parent) { }

void Renderer::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void Renderer::extractMaterials(const nlohmann::json& json)
{
	if (json.is_array()) {
		clearMaterials();

		for (const auto& m : json) {
			addMaterial(Content::instance()->getPooledFromJson<Material>(m));
		}
	}
}
