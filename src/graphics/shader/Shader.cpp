#include "graphics/shader/Shader.hpp"
#include "util/shader_preprocessor.hpp"
#include "util/type_registry.hpp"

REGISTER_OBJECT_TYPE(Shader, "shader", ".glsl");

Shader::Shader(shader_type type, const std::string& source) : m_glObj(0), m_type(type), m_compilerStatus(GL_FALSE)
{
	const char* csource = source.c_str();

	m_glObj = glCreateShader(m_type);
	glShaderSource(m_glObj, 1, &csource, 0);
	glCompileShader(m_glObj);

	glGetShaderiv(m_glObj, GL_COMPILE_STATUS, &m_compilerStatus);

	GLint logLength;
	glGetShaderiv(m_glObj, GL_INFO_LOG_LENGTH, &logLength);

	auto log = std::make_unique<GLchar[]>(logLength);
	glGetShaderInfoLog(m_glObj, logLength, &logLength, log.get());
	m_compilerLog.assign(log.get());
}

Shader::~Shader()
{
	if (m_glObj) glDeleteShader(m_glObj);
}

Shader::Shader(Shader&& other) : m_glObj(other.m_glObj), m_compilerStatus(other.m_compilerStatus), m_compilerLog(std::move(other.m_compilerLog))
{
	other.m_glObj = 0;
	other.m_compilerStatus = GL_FALSE;
}

Shader& Shader::operator=(Shader&& other)
{
	if (this != &other) {
		m_glObj = other.m_glObj;
		m_compilerStatus = other.m_compilerStatus;
		m_compilerLog = std::move(other.m_compilerLog);

		other.m_glObj = 0;
		other.m_compilerStatus = GL_FALSE;
	}

	return *this;
}

GLuint Shader::getObj() const
{
	return m_glObj;
}

bool Shader::hasCompilerErrors() const
{
	return m_compilerStatus == GL_FALSE;
}

const std::string& Shader::getLog() const
{
	return m_compilerLog;
}


template<>
std::unique_ptr<Shader> import_object<Shader>(std::istream& stream)
{
	shader_preprocessor pp(stream);
	if (pp) {
		return std::make_unique<Shader>(pp.shader_type(), pp.processed_src());
	}
	return std::unique_ptr<Shader>();
}

template<>
std::unique_ptr<Shader> import_object<Shader>(const path& fname)
{
	shader_preprocessor pp(fname);
	if (pp) {
		return std::make_unique<Shader>(pp.shader_type(), pp.processed_src());
	}
	return std::unique_ptr<Shader>();
}
