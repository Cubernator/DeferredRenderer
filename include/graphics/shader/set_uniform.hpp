#ifndef SET_UNIFORM_HPP
#define SET_UNIFORM_HPP

#include "glm.hpp"
#include "GL/glew.h"

inline void set_uniform(GLint location, float value)
{
	glUniform1f(location, value);
}

inline void set_uniform(GLint location, const glm::vec2& value)
{
	glUniform2fv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::vec3& value)
{
	glUniform3fv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::vec4& value)
{
	glUniform4fv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, int value)
{
	glUniform1i(location, value);
}

inline void set_uniform(GLint location, const glm::ivec2& value)
{
	glUniform2iv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::ivec3& value)
{
	glUniform3iv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::ivec4& value)
{
	glUniform4iv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, unsigned int value)
{
	glUniform1ui(location, value);
}

inline void set_uniform(GLint location, const glm::uvec2& value)
{
	glUniform2uiv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::uvec3& value)
{
	glUniform3uiv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::uvec4& value)
{
	glUniform4uiv(location, 1, &value[0]);
}

inline void set_uniform(GLint location, const glm::mat4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

inline void set_uniform(GLint location, const glm::mat3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

inline void set_uniform(GLint location, const glm::mat4x3& value)
{
	glUniformMatrix4x3fv(location, 1, GL_FALSE, &value[0][0]);
}

#endif // SET_UNIFORM_HPP