#include "Content.hpp"
#include "pooled.hpp"
#include "core/app_info.hpp"
#include "scripting/class_registry.hpp"

#include "boost/filesystem.hpp"
#include "boost/format.hpp"

#include <algorithm>

Content::Content() :
	m_contentRoot(app_info::get<path>("contentRoot", "content")),
	m_shaderIncludeDirs(app_info::get<std::vector<path>>("shaderIncludeDirs")),
	m_logSearch(app_info::get<bool>("logContentSearch", false)),
	m_logIndentLevel(0), m_anonCounter(0)
{
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

bool Content::findObject(std::type_index tid, const std::string& name, path& result)
{
	auto it1 = m_registry.find(tid);
	if (it1 != m_registry.end()) {

		auto tr = it1->second;
		auto it2 = tr.find(name);
		if (it2 != tr.end()) {
			result =  it2->second;
			return true;
		}
	}

	return false;
}

std::unique_ptr<NamedObject> Content::getFromDisk(const object_type& type, const std::string& name)
{
	return getFromDiskInt<NamedObject>(type, name);
}

std::unique_ptr<NamedObject> Content::getFromJson(const object_type& type, const nlohmann::json& json)
{
	return getFromJsonInt<NamedObject>(type, json);
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

	if (m_logSearch) {
		for (unsigned int i = 0; i < m_logIndentLevel; ++i) std::cout << "  ";
		std::cout << p.filename().string() << ": ";
	}

	if (is_directory(p)) {
		if (m_logSearch) std::cout << std::endl;

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
	object_type type = type_registry::findByExtension(ext);

	if (type) {
		name = p.stem().string();
	} else {
		try {
			boost::filesystem::ifstream f(p);
			if (f) {
				nlohmann::json j;
				j << f;
				if (j.is_object()) {
					std::string tn = get_type(j);
					type = type_registry::findByName(tn);
					name = get_name(j);

					if (!type) {
						std::cout << "WARNING: found json file with unknown object type: \"" << tn << "\"";
						if (!m_logSearch) std::cout << std::endl;
					}
				}
			}
		} catch (std::invalid_argument&) {
			m_genericRegistry.emplace(p.filename().string(), p);
			if (m_logSearch) std::cout << "found generic file";
		}
	}

	if (type) {
		m_registry[type.id][name] = p;
		if (m_logSearch) std::cout << "found " << type.name << " \"" << name << "\"";
	}

	if (m_logSearch) std::cout << std::endl;
}

std::string Content::getAnonName()
{
	auto fmt = boost::format("unnamed_%1$4i") % (m_anonCounter++);
	return fmt.str();
}

std::string Content::getObjectName(const nlohmann::json& json)
{
	std::string result;
	if (!get_value(json, "name", result)) {
		result = getAnonName();
	}
	return result;
}

SCRIPTING_REGISTER_STATIC_CLASS(Content)

SCRIPTING_DEFINE_METHOD(Content, get)
{
	scripting::push_value(L,
		content::get_pooled(
			scripting::check_arg<std::string>(L, 1),
			scripting::check_arg<std::string>(L, 2)
		)
	);
	return 1;
}
