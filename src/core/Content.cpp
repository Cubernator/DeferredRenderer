#include "core/Content.hpp"
#include "util/type_registry.hpp"
#include "util/app_info.hpp"

#include "boost/filesystem.hpp"

#include <algorithm>

Content* Content::s_instance = nullptr;

Content::Content() : m_logIndentLevel(0)
{
	s_instance = this;

	m_contentRoot = app_info::get<path>("contentRoot", "content");
	m_shaderIncludeDirs = app_info::get<std::vector<path>>("shaderIncludeDirs");

	std::cout << "scanning content..." << std::endl;
	scanContentFolder(m_contentRoot);
}

path Content::findGenericFirst(const std::string& name)
{
	path result;
	findGenericFirst(name, result);
	return result;
}

bool Content::findGenericFirst(const std::string& name, path& result)
{
	auto it = m_genericRegistry.find(name);
	if (it != m_genericRegistry.end()) {
		result = it->second;
		return true;
	}

	std::cout << "could not find generic file \"" << name << "\"" << std::endl;

	return false;
}

std::vector<path> Content::findGenericAll(const std::string& name)
{
	std::vector<path> result;
	auto range = m_genericRegistry.equal_range(name);
	for (auto it = range.first; it != range.second; ++it) {
		result.push_back(it->second);
	}
	return std::move(result);
}

path Content::findObject(const std::type_info& type, const std::string& name)
{
	auto it1 = m_registry.find(type);
	if (it1 != m_registry.end()) {

		auto tr = it1->second;
		auto it2 = tr.find(name);
		if (it2 != tr.end()) {
			return it2->second;
		}
	}

	auto& t = type_registry::findByType(type);
	std::cout << "could not find object \"" << name << "\" of type \"" << t.name << "\"" << std::endl;

	return "";
}

bool Content::findShaderFile(const path& p, path& result) const
{
	path tmp;
	if (p.is_relative()) {
		for (const path& dir : m_shaderIncludeDirs) {
			tmp = m_contentRoot / dir / p;
			if (boost::filesystem::exists(tmp)) {
				result = std::move(tmp);
				return true;
			}
		}
	}

	return false;
}

void Content::scanContentFolder(const path& p)
{
	using namespace boost::filesystem;

	for (unsigned int i = 0; i < m_logIndentLevel; ++i) std::cout << "  ";
	std::cout << p.filename().string() << ": ";

	if (is_directory(p)) {
		std::cout << std::endl;

		++m_logIndentLevel;

		for (auto dit = directory_iterator(p); dit != directory_iterator(); ++dit) {
			scanContentFolder(dit->path());
		}

		--m_logIndentLevel;
	} else if (is_regular_file(p)) {
		addFile(p);
	}
}

void Content::addFile(const path& p)
{
	std::string name;

	std::string ext = p.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
	registered_type type = type_registry::findByExtension(ext);

	if (type) {
		name = p.stem().string();
	} else {
		try {
			boost::filesystem::ifstream f(p);
			if (f) {
				nlohmann::json j;
				j << f;
				if (j.is_object()) {
					type = type_registry::findByName(get_type(j));
					name = get_name(j);
				}
			}
		} catch (std::invalid_argument&) {
			m_genericRegistry.emplace(p.filename().string(), p);
			std::cout << "found generic file";
		}
	}

	if (type) {
		m_registry[type.type][name] = p;
		std::cout << "found " << type.name << " \"" << name << "\"";
	}

	std::cout << std::endl;
}

