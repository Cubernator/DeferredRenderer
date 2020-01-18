#ifndef GRAPHICS_SHADERPROGRAM_HPP
#define GRAPHICS_SHADERPROGRAM_HPP

#include "core/NamedObject.hpp"
#include "util/import.hpp"

#include "glm.hpp"
#include "set_uniform.hpp"
#include "uniform_id.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace hexeract
{
	namespace graphics
	{
		class Shader;
		class Texture;

		class ShaderProgram : public NamedObject
		{
		public:
			template<typename Iter>
			ShaderProgram(Iter first, Iter last) : m_glObj(0), m_shaders(first, last), m_linkerStatus(GL_FALSE), m_good(false)
			{
				init();
			}

			explicit ShaderProgram(std::initializer_list<Shader*> shaders) : ShaderProgram(shaders.begin(), shaders.end()) { }

			~ShaderProgram();

			ShaderProgram(ShaderProgram&& other);
			ShaderProgram& operator=(ShaderProgram&& other);

			GLuint glObj() const { return m_glObj; }

			GLint linkerStatus() const { return m_linkerStatus; }
			const std::string& linkerLog() const { return m_linkerLog; }

			bool hasLinkerErrors() const { return m_linkerStatus == GL_FALSE; }

			bool isGood() const { return m_good; }
			operator bool() const { return isGood(); }

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

		GLenum sampler_type_to_target(GLenum type);
	}

	template<>
	std::unique_ptr<graphics::ShaderProgram> json_to_object<graphics::ShaderProgram>(const nlohmann::json& json);
}

#endif // GRAPHICS_SHADERPROGRAM_HPP