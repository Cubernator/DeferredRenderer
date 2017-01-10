#ifndef GL_TYPES_HPP
#define GL_TYPES_HPP

#include "GL/glew.h"

#include <cstdint>

template<typename> inline constexpr GLenum gl_type() { return 0; }

template<> inline constexpr GLenum gl_type<uint8_t>() { return GL_UNSIGNED_BYTE; }
template<> inline constexpr GLenum gl_type<uint16_t>() { return GL_UNSIGNED_SHORT; }
template<> inline constexpr GLenum gl_type<uint32_t>() { return GL_UNSIGNED_INT; }

template<> inline constexpr GLenum gl_type<int8_t>() { return GL_BYTE; }
template<> inline constexpr GLenum gl_type<int16_t>() { return GL_SHORT; }
template<> inline constexpr GLenum gl_type<int32_t>() { return GL_INT; }

template<> inline constexpr GLenum gl_type<float>() { return GL_FLOAT; }
template<> inline constexpr GLenum gl_type<double>() { return GL_DOUBLE; }


#endif // GL_TYPES_HPP