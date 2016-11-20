#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"
#include "util/bounds.hpp"

class Material;

class Renderer : public Component
{
public:
	Renderer(Entity* parent);

	Material* getMaterial() { return m_material; }
	const Material* getMaterial() const { return m_material; }

	void setMaterial(Material* material) { m_material = material; }

	void bind() { bind_impl(); }
	void unbind() { unbind_impl(); }
	void draw() { draw_impl(); }

	bool isVisible() const { return hasGeometry(); }

	aabb getBounds() const { return getBounds_impl(); }

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void bind_impl() { }
	virtual void unbind_impl() { }

	virtual void draw_impl() = 0;
	virtual bool hasGeometry() const = 0;

	virtual aabb getBounds_impl() const = 0;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	Material* m_material;

	static json_interpreter<Renderer> s_properties;

	void extractMaterial(const nlohmann::json& json);
};

#endif // RENDERER_HPP