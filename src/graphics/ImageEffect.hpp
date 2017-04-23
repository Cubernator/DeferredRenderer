#ifndef IMAGEEFFECT_HPP
#define IMAGEEFFECT_HPP

#include "core/Component.hpp"

class Texture2D;
class RenderTexture;

class ImageEffect : public Component
{
public:
	ImageEffect(Entity* parent) : Component(parent) { }

	virtual void apply(const Texture2D* input, const RenderTexture* output) = 0;

	COMPONENT_ALLOW_MULTIPLE;
};

#endif //IMAGEEFFECT_HPP