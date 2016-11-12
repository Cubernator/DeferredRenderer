#include "graphics/ShaderProgram.hpp"
#include "core/Content.hpp"

#include <algorithm>
#include <numeric>

REGISTER_OBJECT_TYPE_DEF_NO_EXT(ShaderProgram, "shaderProgram");

bool ShaderProgram::getTarget(GLenum samplerType, GLenum& target)
{
	switch (samplerType) {
	case GL_SAMPLER_1D:
		target = GL_TEXTURE_1D;
		return true;
	case GL_SAMPLER_2D:
		target = GL_TEXTURE_2D;
		return true;
	case GL_SAMPLER_3D:
		target = GL_TEXTURE_3D;
		return true;
	case GL_SAMPLER_CUBE:
		target = GL_TEXTURE_CUBE_MAP;
		return true;
	case GL_SAMPLER_2D_RECT:
		target = GL_TEXTURE_RECTANGLE;
		return true;
	}

	return false;
}

ShaderProgram::~ShaderProgram()
{
	if (m_glObj) glDeleteProgram(m_glObj);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
	: m_glObj(other.m_glObj),
	m_shaders(std::move(other.m_shaders)),
	m_uniforms(std::move(other.m_uniforms)),
	m_textures(std::move(other.m_textures)),
	m_linkerStatus(other.m_linkerStatus),
	m_linkerLog(std::move(other.m_linkerLog)),
	m_good(other.m_good)
{
	other.m_glObj = 0;
	other.m_linkerStatus = GL_FALSE;
	other.m_good = false;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
	if (this != &other) {
		m_glObj = other.m_glObj;
		m_shaders = std::move(other.m_shaders);
		m_uniforms = std::move(other.m_uniforms);
		m_textures = std::move(other.m_textures);
		m_linkerStatus = other.m_linkerStatus;
		m_linkerLog = std::move(other.m_linkerLog);
		m_good = other.m_good;

		other.m_glObj = 0;
		other.m_linkerStatus = GL_FALSE;
		other.m_good = false;
	}

	return *this;
}

void ShaderProgram::init()
{
	m_glObj = glCreateProgram();

	for (auto& shader : m_shaders) {
		glAttachShader(m_glObj, shader->getObj());
	}

	glLinkProgram(m_glObj);

	glGetProgramiv(m_glObj, GL_LINK_STATUS, &m_linkerStatus);

	GLint logLength;
	glGetProgramiv(m_glObj, GL_INFO_LOG_LENGTH, &logLength);

	auto log = std::make_unique<GLchar[]>(logLength);
	glGetProgramInfoLog(m_glObj, logLength, &logLength, log.get());
	m_linkerLog.assign(log.get());

	m_good = !(hasLinkerErrors() || std::any_of(m_shaders.begin(), m_shaders.end(), [](Shader* shader) {
		return shader->hasCompilerErrors();
	}));

	if (isGood()) getUniforms();
}

void ShaderProgram::getUniforms()
{
	GLint uniformCount, maxLength;

	glGetProgramiv(m_glObj, GL_ACTIVE_UNIFORMS, &uniformCount);
	glGetProgramiv(m_glObj, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	unsigned int unitCounter = 0;

	auto buf = std::make_unique<GLchar[]>(maxLength);

	GLsizei l;
	GLint s;
	GLenum t;

	GLenum texTarget;
	GLuint loc;

	for (int i = 0; i < uniformCount; ++i) {
		glGetActiveUniform(m_glObj, i, maxLength, &l, &s, &t, buf.get());
		std::string name(buf.get());

		loc = glGetUniformLocation(m_glObj, buf.get());

		if (getTarget(t, texTarget)) {
			m_textures[name] = ShaderTextureUnit(unitCounter++, texTarget, loc);
		} else {
			m_uniforms[name] = loc;
		}
	}
}

GLuint ShaderProgram::getObj() const
{
	return m_glObj;
}

bool ShaderProgram::hasLinkerErrors() const
{
	return m_linkerStatus == GL_FALSE;
}

const std::string& ShaderProgram::getLog() const
{
	return m_linkerLog;
}

bool ShaderProgram::isGood() const
{
	return m_good;
}

ShaderProgram::operator bool() const
{
	return isGood();
}

GLint ShaderProgram::getUniformLoc(const std::string & name) const
{
	GLint output = -1;
	getUniformLoc(name, output);
	return output;
}

bool ShaderProgram::getUniformLoc(const std::string & name, GLint & loc) const
{
	if (m_uniforms.count(name)) {
		loc = m_uniforms.at(name);
		return true;
	}

	return false;
}

bool ShaderProgram::getTextureUnit(const std::string & name, ShaderTextureUnit & unit) const
{
	if (m_textures.count(name)) {
		unit = m_textures.at(name);
		return true;
	}

	return false;
}

void ShaderProgram::bind()
{
	if (isGood()) glUseProgram(m_glObj);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

void ShaderProgram::setTexture(const std::string & name, GLuint tex)
{
	ShaderTextureUnit unit;
	if (getTextureUnit(name, unit)) {
		glUniform1i(unit.location, unit.unit);
		glActiveTexture(GL_TEXTURE0 + unit.unit);
		glBindTexture(unit.target, tex);
	}
}

template<>
std::unique_ptr<ShaderProgram> json_to_object<ShaderProgram>(const nlohmann::json& json)
{
	if (json.is_structured()) {
		nlohmann::json shaderList;
		if (json.is_array()) {
			shaderList = json;
		} else {
			auto it = json.find("shaders");
			if (it != json.end() && it->is_structured()) {
				shaderList = *it;
			}
		}

		std::vector<Shader*> shaders;
		for (auto& j : shaderList) {
			if (j.is_string()) {
				Shader* s = Content::instance()->getPooledFromDisk<Shader>(j.get<std::string>());
				if (s) shaders.push_back(s);
			}
		}
		return std::make_unique<ShaderProgram>(shaders.begin(), shaders.end());
	}

	return std::unique_ptr<ShaderProgram>();
}