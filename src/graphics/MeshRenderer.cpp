#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "scripting/class_registry.hpp"
#include "core/component_registry.hpp"
#include "core/Entity.hpp"
#include "content/pooled.hpp"

namespace hexeract
{
	namespace graphics
	{
		REGISTER_COMPONENT_CLASS(MeshRenderer);

		json_interpreter<MeshRenderer> MeshRenderer::s_properties({
			{ "mesh", &MeshRenderer::extractMesh }
		});

		MeshRenderer::MeshRenderer(Entity* parent) : Renderer(parent), m_mesh(nullptr) { }

		bool MeshRenderer::hasGeometry() const
		{
			return m_mesh != nullptr;
		}

		const Drawable* MeshRenderer::getDrawable_impl(std::size_t index) const
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
			setMesh(content::get_pooled<Mesh>(json));
		}

		SCRIPTING_REGISTER_DERIVED_CLASS(MeshRenderer, Renderer);

		SCRIPTING_AUTO_METHOD(MeshRenderer, mesh);
		SCRIPTING_AUTO_METHOD(MeshRenderer, setMesh);
	}
}