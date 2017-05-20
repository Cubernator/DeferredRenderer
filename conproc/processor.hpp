#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "boost/filesystem.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <vector>

namespace fs = boost::filesystem;

class conproc;

class processor
{
public:
	explicit processor(const conproc* parent) : m_parent(parent) { }

	virtual ~processor() { }

	void process(const fs::path& file);

protected:
	const conproc* const m_parent;

	virtual void process_impl(const fs::path& file, const nlohmann::json& options) = 0;
};

template<typename T>
struct construct_processor
{
	std::unique_ptr<T> operator() (const conproc* p) const
	{
		return std::make_unique<T>(p);
	}
};

class processor_factory
{
public:
	using factory_fun = std::function<std::unique_ptr<processor>(const conproc*)>;

	processor_factory(const conproc* parent, factory_fun fun, const std::string& typeName, const std::string& helpMsg, const std::vector<std::string>& extensions)
		: m_parent(parent), m_fun(fun), m_typeName(typeName), m_helpMsg(helpMsg), m_extensions(extensions) { }

	const std::string& type_name() const { return m_typeName; }
	const std::string& help_msg() const { return m_helpMsg; }
	const std::vector<std::string>& extensions() const { return m_extensions; }

	std::unique_ptr<processor> get_processor() const
	{
		return m_fun(m_parent);
	}

private:
	const conproc* m_parent;
	factory_fun m_fun;
	std::string m_typeName;
	std::string m_helpMsg;
	std::vector<std::string> m_extensions;
};

template<typename T>
processor_factory make_processor_factory(const conproc* parent, const std::string& typeName, const std::string& helpMsg, const std::vector<std::string>& extensions)
{
	return processor_factory(parent, construct_processor<T>(), typeName, helpMsg, extensions);
}

#endif // PROCESSOR_HPP