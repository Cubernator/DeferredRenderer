#ifndef TEXTURE_UNIT_MANAGER_HPP
#define TEXTURE_UNIT_MANAGER_HPP

#include "GL/glew.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/indexed_by.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/sequenced_index.hpp"

namespace mi = boost::multi_index;

class Texture;

class texture_unit_manager
{
public:
	static void emptyCache();

private:
	struct tex_unit
	{
		const Texture* currentTexture;
		GLuint unit;

		void bind() const;
	};

	struct by_texture { };
	struct by_usage { };

	struct unit_cache_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_texture>, mi::member<tex_unit, const Texture*, &tex_unit::currentTexture>>,
		mi::sequenced<mi::tag<by_usage>>
	> { };

	using unit_cache = mi::multi_index_container<tex_unit, unit_cache_indices>;

	static GLuint s_maxUnits, s_nextUnit;
	static unit_cache s_cache;

	static void init();

	static GLint bindTexture(const Texture* texture);
};

#endif // TEXTURE_UNIT_MANAGER_HPP