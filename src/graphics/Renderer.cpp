#include "Renderer.hpp"
#include "Material.hpp"
#include "content/Content.hpp"

json_interpreter<Renderer> Renderer::s_properties({
	{ "materials", &Renderer::extractMaterials }
});


Renderer::Renderer(Entity* parent) : Component(parent) { }

void Renderer::apply_json_impl(const nlohmann::json& json)
{
	Component::apply_json_impl(json);
	s_properties.interpret_all(this, json);
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
