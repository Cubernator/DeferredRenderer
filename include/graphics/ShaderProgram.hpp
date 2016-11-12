#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <memory>
#include <unordered_map>

#include "Shader.hpp"
#include "glm.hpp"
#include "set_uniform.hpp"

struct ShaderTextureUnit
{
	unsigned int unit;
	GLenum target;
	GLint location;

	ShaderTextureUnit() = default;
	ShaderTextureUnit(unsigned int unit, GLenum target, GLint location) : unit(unit), target(target), location(location) { }
};

class ShaderProgram
{
public:
	template<typename Iter>
	ShaderProgram(Iter first, Iter last) : m_glObj(0), m_shaders(first, last), m_linkerStatus(GL_FALSE), m_good(false)
	{
		init();
	}

	ShaderProgram(std::initializer_list<Shader*> shaders) : ShaderProgram(shaders.begin(), shaders.end()) { }

	~ShaderProgram();

	ShaderProgram(ShaderProgram&& other);
	ShaderProgram& operator=(ShaderProgram&& other);

	GLuint getObj() const;

	bool hasLinkerErrors() const;
	const std::string& getLog() const;

	bool isGood() const;

	operator bool() const;

	GLint getUniformLoc(const std::string& name) const;
	bool getUniformLoc(const std::string& name, GLint& loc) const;

	bool getTextureUnit(const std::string& name, ShaderTextureUnit& unit) const;

	void bind();
	void unbind();

	template<typename T>
	void setUniform(const std::string& name, const T& value)
	{
		GLint loc;
		if (getUniformLoc(name, loc)) {
			set_uniform(loc, value);
		}
	}

	void setTexture(const std::string& name, GLuint tex);

private:
	GLuint m_glObj;
	std::vector<Shader*> m_shaders;

	std::unordered_map<std::string, GLint> m_uniforms;
	std::unordered_map<std::string, ShaderTextureUnit> m_textures;

	GLint m_linkerStatus;
	std::string m_linkerLog;
	bool m_good;

	void init();
	void getUniforms();

	static bool getTarget(GLenum samplerType, GLenum& target);

	REGISTER_OBJECT_TYPE_DECL(ShaderProgram);
};

template<>
std::unique_ptr<ShaderProgram> json_to_object<ShaderProgram>(const nlohmann::json& json);


#endif // SHADERPROGRAM_HPP