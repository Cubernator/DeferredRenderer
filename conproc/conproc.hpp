#ifndef CONPROC_HPP
#define CONPROC_HPP

#include "boost/filesystem.hpp"

#include "processor.hpp"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <ostream>

namespace fs = boost::filesystem;

struct content_file
{
	std::string type;
	fs::path path;
};

class conproc
{
public:
	conproc();

	int process(int argc, char* argv[]);

	template<typename T>
	void register_processor(const std::string& typeName, const std::string& helpMsg, const std::vector<std::string>& extensions)
	{
		register_processor(make_processor_factory<T>(this, typeName, helpMsg, extensions));
	}

	void register_processor(processor_factory factory);

	bool is_verbose() const { return m_verbose; }
	const fs::path& dest_dir() const { return m_destDir; }

private:
	bool m_verbose;
	fs::path m_destDir;
	std::vector<content_file> m_input;
	std::map<std::string, processor_factory> m_processors;
	std::map<std::string, std::string> m_extensions;

	std::string get_type(const fs::path& file);
};

std::ostream& debug_output();

#endif // CONPROC_HPP