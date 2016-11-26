#include "util/shader_preprocessor.hpp"
#include "core/Content.hpp"

#include "boost/format.hpp"
#include "boost/spirit/home/qi.hpp"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"

#include <iostream>

shader_preprocessor::shader_preprocessor(const path& fname, unsigned int srcId) : m_filename(fname), m_shaderType(Shader::type_undefined), m_error(false), m_srcId(srcId)
{
	boost::filesystem::ifstream fs(fname);
	if (!fs) {
		std::cout << "Shader preprocessor error: could not open file " << fname << std::endl;
		m_error = true;
	} else {
		process(fs);
	}
}

shader_preprocessor::shader_preprocessor(std::istream& stream, unsigned int srcId) : m_shaderType(Shader::type_undefined), m_error(false), m_srcId(srcId)
{
	process(stream);
}

bool shader_preprocessor::good() const
{
	return m_shaderType != Shader::type_undefined && !m_error;
}



using namespace boost::spirit::qi;


using pp_include = path;

using pp_pragma_type = Shader::shader_type;
using pp_pragma = pp_pragma_type;

using pp_line = boost::variant<pp_include, pp_pragma>;


template<typename Iterator>
struct pp_include_parser : grammar<Iterator, pp_include(), space_type>
{
	pp_include_parser() : base_type(start)
	{
		q_string %= '\"' >> no_skip[+(char_ - '\"')] >> '\"';
		h_string %= '<' >> no_skip[+(char_ - char_("<>"))] >> '>';

		start %= lit("include") >> (q_string | h_string);
	}

	rule<Iterator, std::string(), space_type> q_string;
	rule<Iterator, std::string(), space_type> h_string;
	rule<Iterator, pp_include(), space_type> start;
};


template<typename Iterator>
struct pp_pragma_parser : grammar<Iterator, pp_pragma(), space_type>
{
	pp_pragma_parser() : base_type(start)
	{
		types.add
			("vertex",			Shader::type_vertex)
			("fragment",		Shader::type_fragment)
			("geometry",		Shader::type_geometry)
			("tess_control",	Shader::type_tess_control)
			("tess_evaluation", Shader::type_tess_evaluation)
			("compute",			Shader::type_compute);

		fn_type %= lit("type") >> types;

		start %= lit("pragma") >> fn_type;
	}

	symbols<char, Shader::shader_type> types;

	rule<Iterator, pp_pragma_type(), space_type> fn_type;
	rule<Iterator, pp_pragma(), space_type> start;
};


template<typename Iterator>
struct pp_line_parser : grammar<Iterator, pp_line(), space_type>
{
	pp_line_parser() : base_type(start)
	{
		directives %= '#' >> (include | pragma);
		start %= directives;
	}

	pp_include_parser<Iterator> include;
	pp_pragma_parser<Iterator> pragma;

	rule<Iterator, pp_line(), space_type> directives;
	rule<Iterator, pp_line(), space_type> start;
};

struct pp_line_visitor : boost::static_visitor<bool>
{
	shader_preprocessor *pp;

	pp_line_visitor(shader_preprocessor *pp) : pp(pp) { }

	bool operator() (const pp_include& include) const {
		pp->include(include);
		return false;
	}

	bool operator() (const pp_pragma& pragma) const {
		pp->pragma_type(pragma);
		return true;
	}
};

void shader_preprocessor::process(std::istream& stream)
{
	m_currentLine = 1;
	m_nextSrcId = m_srcId + 1;

	if (!stream) return;

	pp_line_parser<std::string::const_iterator> pp_line_;

	std::string line_orig;
	while (std::getline(stream, line_orig)) {
		line_orig.push_back('\n');
		m_original.append(line_orig);

		bool appendOrig = true;
		pp_line parsed_line;
		if (phrase_parse(line_orig.cbegin(), line_orig.cend(), pp_line_, space, parsed_line)) {
			appendOrig = boost::apply_visitor(pp_line_visitor(this), parsed_line);
		}

		if (m_error)
			break;

		if (appendOrig)
			m_processed.append(line_orig);
		++m_currentLine;
	}
}

void shader_preprocessor::include(const path& file)
{
	using namespace boost::filesystem;

	bool found = false;
	path filepath;
	if (file.is_absolute()) {
		filepath = file;
		found = exists(filepath);
	} else {
		filepath = m_filename.parent_path() / file;
		found = exists(filepath);
		if (!found) {
			found = Content::instance()->findShaderFile(file, filepath);
		}
	}

	if (!found) {
		std::cout << m_filename.string() << "(" << m_currentLine << "): could not find include file " << file << std::endl;
		m_error = true;
		return;
	}

	shader_preprocessor pp(filepath, m_nextSrcId);
	if (pp.m_error) {
		m_error = true;
		return;
	}

	auto fmt = boost::format("#line %i %i\n");

	m_processed.append((fmt % 1 % m_nextSrcId).str());
	m_processed.append(pp.processed_src());
	m_processed.append((fmt % (m_currentLine+1) % m_srcId).str());

	m_nextSrcId = pp.m_nextSrcId;

	auto st = pp.shader_type();
	if (st) {
		if (m_shaderType) {
			// TODO: print warning about multiple shader type definitions
		}
		pragma_type(st);
	}
}

void shader_preprocessor::pragma_type(Shader::shader_type type)
{
	m_shaderType = type;
}
