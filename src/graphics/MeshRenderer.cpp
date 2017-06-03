#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "util/component_registry.hpp"
#include "core/Entity.hpp"
#include "content/Content.hpp"

REGISTER_COMPONENT_CLASS(MeshRenderer, "meshRenderer");

json_interpreter<MeshRenderer> MeshRenderer::s_properties({
	{ "mesh", &MeshRenderer::extractMesh }
});

MeshRenderer::MeshRenderer(Entity* parent) : Renderer(parent), m_mesh(nullptr) { }

bool MeshRenderer::hasGeometry() const
{
	return m_mesh != nullptr;
}

const Drawable* MeshRenderer::getDrawable_impl(unsigned int index) const
{
	if (index >= m_mesh->subMeshCount())
		return nullptr;
	
	return m_mesh->getSubMesh(index);
}

void MeshRenderer::apply_json_impl(const nlohmann::json& json)
{
	Renderer::apply_json_impl(json);
	s_properties.interpret_all(this, json);
}

void MeshRenderer::extractMesh(const nlohmann::json& json)
{
	setMesh(Content::instance()->getPooledFromDisk<Mesh>(json.get<std::string>()));
}

