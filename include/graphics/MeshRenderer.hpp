#ifndef MESHRENDERER_HPP
#define MESHRENDERER_HPP

#include "Renderer.hpp"
#include "util/json_interpreter.hpp"

class Mesh;

class MeshRenderer : public Renderer
{
public:
	MeshRenderer(Entity* parent);

	Mesh* getMesh() { return m_mesh; }
	const Mesh* getMesh() const { return m_mesh; }

	void setMesh(Mesh* mesh) { m_mesh = mesh; }

protected:
	virtual void bind_impl() override;
	virtual void draw_impl() override;
	virtual void unbind_impl() override;
	virtual bool hasGeometry() const override;
	virtual aabb getBounds_impl() const override;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	Mesh* m_mesh;

	static json_interpreter<MeshRenderer> s_properties;

	void extractMesh(const nlohmann::json& json);
};

#endif // MESHRENDERER_HPP