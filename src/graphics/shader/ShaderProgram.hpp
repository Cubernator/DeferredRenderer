#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include "glm.hpp"
#include "set_uniform.hpp"
#include "uniform_id.hpp"

#include "util/import.hpp"

class Shader;
class Texture;

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

	bool getUniformLoc(uniform_id id, GLint& loc) const;

	template<typename T>
	void setUniform(uniform_id id, const T& value) const
	{
		setUniformImpl<T>(id, value);
	}

	void setTexture(uniform_id id, const Texture* texture) const;

	void bind() const;
	void unbind() const;

private:
	GLuint m_glObj;
	std::vector<Shader*> m_shaders;

	struct tex_unit
	{
		GLuint unit;
		GLint location;
	};

	std::unordered_map<uniform_id, GLint> m_uniforms;
	std::unordered_map<uniform_id, tex_unit> m_textures;

	GLint m_linkerStatus;
	std::string m_linkerLog;
	bool m_good;

	void init();
	void getUniforms();

	// C++ meta-programming truly is amazing... and so frustratingly obscure at the same time
	template<typename T>
	using is_texture_pointer = std::is_base_of<Texture, std::remove_pointer_t<T>>;

	template<typename T>
	auto setUniformImpl(uniform_id id, const T& value) const -> std::enable_if_t<!is_texture_pointer<T>::value>
	{
		GLint loc;
		if (getUniformLoc(id, loc)) {
			set_uniform(loc, value);
		}
	}

	template<typename T>
	auto setUniformImpl(uniform_id id, const T& value) const -> std::enable_if_t<is_texture_pointer<T>::value>
	{
		setTexture(id, static_cast<Texture*>(value));
	}
};

template<>
std::unique_ptr<ShaderProgram> json_to_object<ShaderProgram>(const nlohmann::json& json);

GLenum sampler_type_to_target(GLenum type);


#endif // SHADERPROGRAM_HPP