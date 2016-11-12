#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include "GL/glew.h"

#include "util/import.hpp"
#include "util/type_registry.hpp"

class Shader
{
public:
	enum shader_type {
		type_vertex = GL_VERTEX_SHADER,
		type_fragment = GL_FRAGMENT_SHADER,
		type_geometry = GL_GEOMETRY_SHADER,
		type_tess_control = GL_TESS_CONTROL_SHADER,
		type_tess_evaluation = GL_TESS_EVALUATION_SHADER,
		type_compute = GL_COMPUTE_SHADER,
		type_undefined = 0
	};

	Shader(shader_type type, const std::string& source);
	~Shader();

	Shader(Shader&& other);
	Shader& operator=(Shader&& other);

	GLuint getObj() const;

	bool hasCompilerErrors() const;
	const std::string& getLog() const;

private:
	GLuint m_glObj;
	shader_type m_type;
	GLint m_compilerStatus;
	std::string m_compilerLog;

	REGISTER_OBJECT_TYPE_DECL(Shader);
};

template<>
std::unique_ptr<Shader> import_object<Shader>(std::istream& stream);

template<>
std::unique_ptr<Shader> import_object<Shader>(const path& fname);

#endif // SHADER_HPP
