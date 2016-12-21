#ifndef MESHRENDERER_HPP
#define MESHRENDERER_HPP

#include "Renderer.hpp"
#include "util/json_interpreter.hpp"

class Mesh;
class SubMesh;

class MeshRenderer : public Renderer
{
public:
	MeshRenderer(Entity* parent);

	Mesh* getMesh() { return m_mesh; }
	const Mesh* getMesh() const { return m_mesh; }

	void setMesh(Mesh* mesh) { m_mesh = mesh; }

protected:
	virtual const Renderable* getRenderable_impl(unsigned int index) const override;
	virtual bool hasGeometry() const override;
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	Mesh* m_mesh;

	static json_interpreter<MeshRenderer> s_properties;

	void extractMesh(const nlohmann::json& json);
};

#endif // MESHRENDERER_HPP