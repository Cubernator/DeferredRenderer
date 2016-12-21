#ifndef SHADER_PREPROCESSOR_HPP
#define SHADER_PREPROCESSOR_HPP

#include <string>
#include <istream>

#include "GL/glew.h"

#include "graphics/shader/Shader.hpp"

class shader_preprocessor
{
public:
	shader_preprocessor(const path& fname, unsigned int srcId = 0);
	shader_preprocessor(std::istream& stream, unsigned int srcId = 0);

	const path& filename() const { return m_filename; }

	const std::string& original_src() const { return m_original; }
	const std::string& processed_src() const { return m_processed; }

	Shader::shader_type shader_type() const { return m_shaderType; }

	operator bool() const { return good(); }

	bool good() const;

private:
	path m_filename;
	std::string m_original;
	std::string m_processed;
	Shader::shader_type m_shaderType;
	bool m_error;
	unsigned int m_currentLine;
	unsigned int m_srcId, m_nextSrcId;

	void process(std::istream& stream);

	void include(const path& file);
	void pragma_type(Shader::shader_type type);

	std::string get_type_macro(Shader::shader_type type) const;

	friend struct pp_line_visitor;
};

#endif // SHADER_PREPROCESSOR_HPP