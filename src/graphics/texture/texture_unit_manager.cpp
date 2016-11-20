#include "graphics/texture/texture_unit_manager.hpp"
#include "graphics/texture/Texture.hpp"

GLuint texture_unit_manager::s_maxUnits = 0, texture_unit_manager::s_nextUnit = 0;
texture_unit_manager::unit_cache texture_unit_manager::s_cache;

void texture_unit_manager::init()
{
	if (s_maxUnits == 0) {
		GLint val;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &val);
		s_maxUnits = GLuint(val);
	}
}

GLint texture_unit_manager::bindTexture(const Texture* texture)
{
	init(); // lazy initialization

	GLuint result;

	auto& by_texture_index = s_cache.get<by_texture>();
	auto& by_usage_index = s_cache.get<by_usage>();

	auto it = by_texture_index.find(texture);
	if (it != by_texture_index.end()) {
		// texture already in cache, therefore already bound to a unit
		// => simply return unit and mark as most recently used (move to front of cache)
		result = it->unit;

		auto it2 = s_cache.project<by_usage>(it);
		by_usage_index.relocate(by_usage_index.begin(), it2);

	} else {
		// texture not in cache, must be bound to an available unit!
		if (s_nextUnit < s_maxUnits) {
			// cache size limit not reached yet
			// => just get the next available unit
			result = s_nextUnit++;
			auto p = by_usage_index.push_front({ texture, result });
			p.first->bind();

		} else {
			// cache is full! (no remaining free texture units)
			// => assign the texture to the most recently used unit (front of the cache)
			auto mruit = by_usage_index.begin();

			// we are safe to assign the texture pointer since it was not found above (non-collision guarantee)
			by_usage_index.modify(mruit, [&texture](tex_unit& u) {
				u.currentTexture = texture;
			});

			// bind the new texture
			mruit->bind();

			result = mruit->unit;

			// NOTE: Since textures are typically used in a cyclic and repeating fashion (same or similar order every frame),
			// overriding the least recently used texture unit would actually cause more texture binding calls.
			// But if you're using a decent graphics card there should be a lot of texture units, so the cache will probably never fill up.
			// (Just clear the cache when loading a new scene)
		}
	}

	return GLint(result);
}

void texture_unit_manager::emptyCache()
{
	// restore starting conditions
	s_cache.clear();
	s_nextUnit = 0;
	glBindTextures(0, s_maxUnits, nullptr);
}

void texture_unit_manager::tex_unit::bind() const
{
	GLuint glObj = currentTexture->glObj();
	glBindTextures(unit, 1, &glObj);
}
