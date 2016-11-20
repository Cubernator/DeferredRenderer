#include "graphics/MeshRenderer.hpp"
#include "graphics/Mesh.hpp"
#include "util/component_registry.hpp"
#include "core/Entity.hpp"
#include "core/Content.hpp"

REGISTER_COMPONENT_CLASS(MeshRenderer, "meshRenderer");

json_interpreter<MeshRenderer> MeshRenderer::s_properties({
	{ "mesh", &MeshRenderer::extractMesh }
});

MeshRenderer::MeshRenderer(Entity* parent) : Renderer(parent) { }

void MeshRenderer::bind_impl()
{
	m_mesh->bindVAO();
	m_mesh->bindIndices();
}

void MeshRenderer::draw_impl()
{
	m_mesh->draw();
}

void MeshRenderer::unbind_impl()
{
	m_mesh->unbindIndices();
	m_mesh->unbindVAO();
}

bool MeshRenderer::hasGeometry() const
{
	return m_mesh != nullptr;
}

aabb MeshRenderer::getBounds_impl() const
{
	if (m_mesh) return m_mesh->getBounds();
	return aabb();
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

