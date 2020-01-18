#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP

#include "core/Component.hpp"
#include "Drawable.hpp"
#include "util/json_interpreter.hpp"
#include "util/bounds.hpp"

namespace hexeract
{
	namespace graphics
	{
		class Material;

		class Renderer : public Component
		{
		public:
			explicit Renderer(Entity* parent);

			std::size_t materialCount() const { return m_materials.size(); }

			Material* material(std::size_t index = 0) { return m_materials[index]; }
			const Material* material(std::size_t index = 0) const { return m_materials[index]; }

			std::vector<Material*> materials() { return m_materials; }

			void setMaterials(std::vector<Material*> mats) { m_materials = mats; }

			void addMaterial(Material* mat) { m_materials.push_back(mat); }
			void clearMaterials() { m_materials.clear(); }

			const Drawable* getDrawable(std::size_t index) const { return getDrawable_impl(index); }

			bool isVisible() const { return hasGeometry(); }

			COMPONENT_DISALLOW_MULTIPLE;

		protected:
			virtual const Drawable* getDrawable_impl(std::size_t index) const = 0;
			virtual bool hasGeometry() const = 0;
			virtual void apply_json_impl(const nlohmann::json& json) override;

		private:
			std::vector<Material*> m_materials;

			static json_interpreter<Renderer> s_properties;

			void extractMaterials(const nlohmann::json& json);
		};
	}
}

#endif // GRAPHICS_RENDERER_HPP