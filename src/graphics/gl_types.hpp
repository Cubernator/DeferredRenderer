#ifndef GRAPHICS_GL_TYPES_HPP
#define GRAPHICS_GL_TYPES_HPP

#include "GL/glew.h"
#include "types.hpp"

namespace hexeract
{
	namespace graphics
	{
		template<typename> inline constexpr GLenum gl_type() { return 0; }

		template<> inline constexpr GLenum gl_type<u8_t>() { return GL_UNSIGNED_BYTE; }
		template<> inline constexpr GLenum gl_type<u16_t>() { return GL_UNSIGNED_SHORT; }
		template<> inline constexpr GLenum gl_type<u32_t>() { return GL_UNSIGNED_INT; }

		template<> inline constexpr GLenum gl_type<s8_t>() { return GL_BYTE; }
		template<> inline constexpr GLenum gl_type<s16_t>() { return GL_SHORT; }
		template<> inline constexpr GLenum gl_type<s32_t>() { return GL_INT; }

		template<> inline constexpr GLenum gl_type<f16_t>() { return GL_HALF_FLOAT; }
		template<> inline constexpr GLenum gl_type<f32_t>() { return GL_FLOAT; }
		template<> inline constexpr GLenum gl_type<f64_t>() { return GL_DOUBLE; }
	}
}

#endif // GRAPHICS_GL_TYPES_HPP