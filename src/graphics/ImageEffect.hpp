#ifndef GRAPHICS_IMAGEEFFECT_HPP
#define GRAPHICS_IMAGEEFFECT_HPP

#include "core/Component.hpp"

namespace hexeract
{
	namespace graphics
	{
		class Texture2D;
		class RenderTexture;

		class ImageEffect : public Component
		{
		public:
			explicit ImageEffect(Entity* parent) : Component(parent) { }

			virtual void apply(const Texture2D* input, const RenderTexture* output) = 0;
			virtual bool isGood() const = 0;

			COMPONENT_ALLOW_MULTIPLE;
		};
	}
}

#endif //GRAPHICS_IMAGEEFFECT_HPP