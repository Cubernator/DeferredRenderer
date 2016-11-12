#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"

class Material;

class Renderer : public Component
{
public:
	Renderer(Entity* parent);

	Material* getMaterial() { return m_material; }
	const Material* getMaterial() const { return m_material; }

	void setMaterial(Material* material) { m_material = material; }

	void render() { drawGeometry(); }
	bool isVisible() const { return hasGeometry(); }

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void drawGeometry() = 0;
	virtual bool hasGeometry() const = 0;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	Material* m_material;

	static json_interpreter<Renderer> s_properties;

	void extractMaterial(const nlohmann::json& json);
};

#endif // RENDERER_HPP