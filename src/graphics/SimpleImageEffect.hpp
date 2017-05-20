#ifndef SIMPLEIMAGEEFFECT_HPP
#define SIMPLEIMAGEEFFECT_HPP

#include "ImageEffect.hpp"

class Material;

class SimpleImageEffect : public ImageEffect
{
public:
	explicit SimpleImageEffect(Entity* parent);

	virtual void apply(const Texture2D* input, const RenderTexture* output) final;

	COMPONENT_ALLOW_MULTIPLE;

private:
	Material* m_material;
	bool m_outputLinear;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) final;

	// cppcheck-suppress unusedPrivateFunction
	void extractMaterial(const nlohmann::json& json);

	static json_interpreter<SimpleImageEffect> s_properties;
};

#endif // SIMPLEIMAGEEFFECT_HPP