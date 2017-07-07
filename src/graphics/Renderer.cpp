#include "Renderer.hpp"
#include "Material.hpp"
#include "content/pooled.hpp"
#include "core/Entity.hpp"
#include "core/component_registry.hpp"
#include "scripting/class_registry.hpp"

REGISTER_ABSTRACT_COMPONENT_CLASS(Renderer)


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
			addMaterial(content::get_pooled_json<Material>(m));
		}
	}
}

SCRIPTING_REGISTER_DERIVED_CLASS(Renderer, Component)

SCRIPTING_AUTO_METHOD(Renderer, materialCount)

SCRIPTING_DEFINE_METHOD(Renderer, material)
{
	auto self = scripting::check_self<Renderer>(L);
	auto i = luaL_optinteger(L, 2, 0);
	scripting::push_value(L, self->material(i));
	return 1;
}

SCRIPTING_AUTO_METHOD(Renderer, materials)
SCRIPTING_AUTO_METHOD(Renderer, setMaterials)

SCRIPTING_AUTO_METHOD(Renderer, addMaterial)
SCRIPTING_AUTO_METHOD(Renderer, clearMaterials)
