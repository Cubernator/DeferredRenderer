#ifndef GRAPHICS_SHADER_HPP
#define GRAPHICS_SHADER_HPP

#include "core/NamedObject.hpp"
#include "util/import.hpp"

#include "GL/glew.h"

#include <string>

namespace hexeract
{
	namespace graphics
	{
		class Shader : public NamedObject
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

			GLuint glObj() const { return m_glObj; }

			GLint compilerStatus() const { return m_compilerStatus; }
			const std::string& compilerLog() const { return m_compilerLog; }

			bool hasCompilerErrors() const { return m_compilerStatus == GL_FALSE; }

		private:
			GLuint m_glObj;
			shader_type m_type;
			GLint m_compilerStatus;
			std::string m_compilerLog;
		};
	}

	template<>
	std::unique_ptr<graphics::Shader> import_object<graphics::Shader>(std::istream& stream);

	template<>
	std::unique_ptr<graphics::Shader> import_object<graphics::Shader>(const path& fname);
}

#endif // GRAPHICS_SHADER_HPP
