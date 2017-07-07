#ifndef MESHRENDERER_HPP
#define MESHRENDERER_HPP

#include "Renderer.hpp"
#include "util/json_interpreter.hpp"

class Mesh;
class SubMesh;

class MeshRenderer : public Renderer
{
public:
	explicit MeshRenderer(Entity* parent);

	Mesh* mesh() { return m_mesh; }
	const Mesh* mesh() const { return m_mesh; }

	void setMesh(Mesh* mesh) { m_mesh = mesh; }

protected:
	virtual const Drawable* getDrawable_impl(std::size_t index) const override;
	virtual bool hasGeometry() const override;
	virtual void apply_json_impl(const nlohmann::json& json) override;

private:
	Mesh* m_mesh;

	static json_interpreter<MeshRenderer> s_properties;

	// cppcheck-suppress unusedPrivateFunction
	void extractMesh(const nlohmann::json& json);
};

#endif // MESHRENDERER_HPP