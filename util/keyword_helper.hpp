#ifndef KEYWORD_HELPER_HPP
#define KEYWORD_HELPER_HPP

#include <unordered_map>
#include <string>

#include "nlohmann/json.hpp"

template<typename T, typename KeyType = std::string>
class keyword_helper
{
public:
	using mapped_type = T;
	using key_type = KeyType;
	using container_type = std::unordered_map<key_type, mapped_type>;
	using value_type = typename container_type::value_type;

	static_assert(std::is_object<key_type>::value, "key type must be an object type! (not a reference or function)");


	explicit keyword_helper(std::initializer_list<value_type> keywords) : m_keywords(keywords) { }

	template<typename IterType>
	keyword_helper(IterType first, IterType last) : m_keywords(first, last) { }

	bool get(const key_type& keyword, mapped_type& value) const
	{
		auto it = m_keywords.find(keyword);
		if (it != m_keywords.end()) {
			value = it->second;
			return true;
		}
		return false;
	}

	bool get(const nlohmann::json& json, mapped_type& value) const
	{
		try {
			return get(json.get<key_type>(), value);
		} catch (std::domain_error&) {
			// TODO: print error
			return false;
		}
	}

	bool findKeyword(const nlohmann::json& json, const std::string& name, mapped_type& value) const
	{
		auto it = json.find(name);
		if (it != json.end()) {
			return get(*it, value);
		}

		return false;
	}

protected:
	container_type m_keywords;
};


#endif // KEYWORD_HELPER_HPP