#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include "graphics/texture/Texture.hpp"
#include "graphics/texture/texture_unit_manager.hpp"
#include "core/type_registry.hpp"
#include "content/pooled.hpp"
#include "scripting/class_registry.hpp"

#include <algorithm>
#include <numeric>

REGISTER_OBJECT_TYPE_NO_EXT(ShaderProgram);

ShaderProgram::~ShaderProgram()
{
	if (m_glObj) glDeleteProgram(m_glObj);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
	: m_glObj(other.m_glObj),
	m_shaders(std::move(other.m_shaders)),
	m_uniforms(std::move(other.m_uniforms)),
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
		glAttachShader(m_glObj, shader->glObj());
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

	auto buf = std::make_unique<GLchar[]>(maxLength);

	GLuint nextUnit = 0;

	GLsizei l;
	GLint s;
	GLenum t;

	for (int i = 0; i < uniformCount; ++i) {
		glGetActiveUniform(m_glObj, i, maxLength, &l, &s, &t, buf.get());
		std::string name(buf.get());
		uniform_id id = uniform_name_to_id(name);

		GLenum texTarget = sampler_type_to_target(t);
		GLint loc = glGetUniformLocation(m_glObj, buf.get());
		
		if (texTarget != 0) {
			m_textures.emplace(id, tex_unit{ nextUnit++, loc });
		} else {
			m_uniforms.emplace(id, loc);
		}
	}
}

bool ShaderProgram::getUniformLoc(uniform_id id, GLint& loc) const
{
	auto it = m_uniforms.find(id);
	if (it != m_uniforms.end()) {
		loc = it->second;
		return true;
	}

	return false;
}

void ShaderProgram::setTexture(uniform_id id, const Texture* texture) const
{
	if (texture) {
		auto it = m_textures.find(id);
		if (it != m_textures.end()) {
			GLuint glObj = texture->glObj();
			const auto& u = it->second;
			glBindTextures(u.unit, 1, &glObj);
			set_uniform(u.location, GLint(u.unit));
		}
	}
}

void ShaderProgram::bind() const
{
	if (isGood()) glUseProgram(m_glObj);
}

void ShaderProgram::unbind() const
{
	glUseProgram(0);
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
				Shader* s = content::get_pooled<Shader>(j);
				if (s) shaders.push_back(s);
			}
		}
		return std::make_unique<ShaderProgram>(shaders.begin(), shaders.end());
	}

	return std::unique_ptr<ShaderProgram>();
}

GLenum sampler_type_to_target(GLenum type)
{
	switch (type) {
	case GL_SAMPLER_1D:
	case GL_SAMPLER_1D_SHADOW:
		return GL_TEXTURE_1D;

	case GL_SAMPLER_2D:
	case GL_SAMPLER_2D_SHADOW:
		return GL_TEXTURE_2D;

	case GL_SAMPLER_3D:
		return GL_TEXTURE_3D;

	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_CUBE_SHADOW:
		return GL_TEXTURE_CUBE_MAP;

	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
		return GL_TEXTURE_1D_ARRAY;

	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
		return GL_TEXTURE_2D_ARRAY;

	case GL_SAMPLER_2D_MULTISAMPLE:
		return GL_TEXTURE_2D_MULTISAMPLE;

	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
		return GL_TEXTURE_RECTANGLE;

	case GL_SAMPLER_BUFFER:
		return GL_TEXTURE_BUFFER;

	default:
		return 0;
	}
}

SCRIPTING_REGISTER_DERIVED_CLASS(ShaderProgram, NamedObject)
