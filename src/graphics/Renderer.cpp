#include "graphics/Renderer.hpp"
#include "graphics/Material.hpp"
#include "core/Content.hpp"

json_interpreter<Renderer> Renderer::s_properties({
	{ "material", &Renderer::extractMaterial }
});


Renderer::Renderer(Entity* parent) : Component(parent) { }

void Renderer::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Component::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void Renderer::extractMaterial(const nlohmann::json& json)
{
	setMaterial(Content::instance()->getPooledFromJson<Material>(json));
}
