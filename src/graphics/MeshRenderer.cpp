#include "graphics/MeshRenderer.hpp"
#include "graphics/Mesh.hpp"
#include "core/Entity.hpp"
#include "core/Content.hpp"

REGISTER_COMPONENT_CLASS_DEF(MeshRenderer, "meshRenderer");

json_interpreter<MeshRenderer> MeshRenderer::s_properties({
	{ "mesh", &MeshRenderer::extractMesh }
});

MeshRenderer::MeshRenderer(Entity* parent) : Renderer(parent) { }

void MeshRenderer::drawGeometry()
{
	m_mesh->draw();
}

bool MeshRenderer::hasGeometry() const
{
	return m_mesh != nullptr;
}

void MeshRenderer::apply_json_property_impl(const std::string& name, const nlohmann::json& json)
{
	Renderer::apply_json_property_impl(name, json);
	s_properties.interpret_property(name, this, json);
}

void MeshRenderer::extractMesh(const nlohmann::json& json)
{
	setMesh(Content::instance()->getPooledFromDisk<Mesh>(json.get<std::string>()));
}

