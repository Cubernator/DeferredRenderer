#ifndef SIMPLEIMAGEEFFECT_HPP
#define SIMPLEIMAGEEFFECT_HPP

#include "ImageEffect.hpp"

class Material;

class SimpleImageEffect : public ImageEffect
{
public:
	explicit SimpleImageEffect(Entity* parent);

	Material* material() { return m_material; }
	const Material* material() const { return m_material; }
	void setMaterial(Material* mat) { m_material = mat; }

	bool isOutputLinear() const { return m_outputLinear; }
	void setOutputLinear(bool val) { m_outputLinear = val; }

	virtual void apply(const Texture2D* input, const RenderTexture* output) final;
	virtual bool isGood() const final;

	COMPONENT_ALLOW_MULTIPLE;

private:
	Material* m_material;
	bool m_outputLinear;

	virtual void apply_json_impl(const nlohmann::json& json) final;

	// cppcheck-suppress unusedPrivateFunction
	void extractMaterial(const nlohmann::json& json);

	static json_interpreter<SimpleImageEffect> s_properties;
};

#endif // SIMPLEIMAGEEFFECT_HPP