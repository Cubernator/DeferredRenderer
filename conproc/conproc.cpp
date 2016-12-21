#include "conproc.hpp"

#include "boost/program_options.hpp"
#include "boost/format.hpp"

#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/null.hpp"

#include <iostream>
#include <algorithm>

#include "generic.hpp"
#include "ignore.hpp"

namespace
{
	bool g_enable_verbose;
	boost::iostreams::stream<boost::iostreams::null_sink> dev_null{ boost::iostreams::null_sink() };
}

conproc::conproc() : m_verbose(false), m_destDir(".")
{
	register_processor<generic_processor>("generic", "no processing, will simply be copied to the destination directory.", {});
	register_processor<ignore_processor>("ignore", "file will be completely ignored.", {".opt"});
}

int conproc::process(int argc, char* argv[])
{
	using namespace boost::program_options;
	using namespace std;

	positional_options_description posDesc;
	posDesc.add("files", -1);

	boost::format descFmt("explicitly specify %s input files");

	options_description visible("Available options");
	visible.add_options()
		("help,h", "print this help message")
		("verbose,v", "enable verbose output")
		("destination,d", value<fs::path>(), "set destination directory (will be created if missing)");

	for (auto& p : m_processors) {
		std::string n = p.second.type_name();
		if (!n.empty()) {
			std::string opt = n + "," + n[0];
			visible.add_options()(opt.c_str(), value<vector<fs::path>>()->multitoken()->composing(), (descFmt % n).str().c_str());
		}
	}

	options_description hidden;
	hidden.add_options()
		("files", value<vector<fs::path>>()->composing());

	options_description all;
	all.add(visible).add(hidden);

	variables_map vm;
	store(command_line_parser(argc, argv).options(all).positional(posDesc).run(), vm);

	notify(vm);

	if (vm.count("help")) {
		cout << "Usage: conproc [files...] [options]" << endl;
		cout << "Processes all specified files depending on their content type (see below)" << endl;
		cout << "and places the output in the destination directory (current working directory by default)." << endl;
		cout << endl;
		cout << visible << endl;

		cout << "Content types:" << endl;

		boost::format helpFmt("  %s: %s");
		for (auto& p : m_processors) {
			cout << (helpFmt % p.first % p.second.help_msg()) << endl;
		}

		cout << endl;
		cout << "The content type is automatically determined for every file in <files> depending on file extension." << endl;
		cout << "If the content type of a certain file cannot be determined, it will be treated as a generic file." << endl;
	}

	if (vm.count("verbose")) {
		m_verbose = true;
	}

	g_enable_verbose = m_verbose;
	debug_output() << "verbose output enabled" << endl;

	auto it = vm.find("destination");
	if (it != vm.end()) {
		m_destDir = it->second.as<fs::path>();
		fs::create_directories(m_destDir);
		debug_output() << "destination set to " << m_destDir << endl;
	}

	it = vm.find("files");
	if (it != vm.end()) {
		auto files = it->second.as<vector<fs::path>>();
		for (auto& file : files) {
			m_input.push_back({ get_type(file), file });
		}
	}

	for (auto& p : m_processors) {
		it = vm.find(p.first);
		if (it != vm.end()) {
			auto files = it->second.as<vector<fs::path>>();
			for (auto& file : files) {
				m_input.push_back({ p.first, file });
			}
		}
	}

	for (auto& file : m_input) {
		auto it = m_processors.find(file.type);
		if (it != m_processors.end()) {
			if (fs::is_regular_file(file.path)) {
				debug_output() << "processing " << file.type << " file: " << file.path << endl;
				auto proc = it->second.get_processor();
				proc->process(file.path);
			} else {
				debug_output() << "WARNING: can't process " << file.path << " which is not a regular file!" << endl;
			}
		}

		debug_output() << std::endl;
	}

	return 0;
}

void conproc::register_processor(processor_factory factory)
{
	const auto& ext = factory.extensions();
	for (auto& e : ext) {
		m_extensions.emplace(e, factory.type_name());
	}

	m_processors.emplace(factory.type_name(), std::move(factory));
}

std::string conproc::get_type(const fs::path& file)
{
	std::string ext = file.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

	auto it = m_extensions.find(ext);
	if (it != m_extensions.end()) {
		return it->second;
	}

	return "generic";
}

std::ostream& debug_output()
{
	if (g_enable_verbose) {
		return std::cout;
	} else {
		return dev_null;
	}
}
