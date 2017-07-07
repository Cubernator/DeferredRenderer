#include "pooled.hpp"

namespace content
{
	NamedObject* get_pooled(const object_type& type, const std::string& name)
	{
		NamedObject* obj = detail::find_int<NamedObject>(type, name);
		if (!obj) {
			auto ptr = instance<Content>()->getFromDisk(type, name);
			if (ptr) {
				return instance<ObjectRegistry>()->add(std::move(ptr));
			}
		}
		return obj;
	}
	NamedObject* get_pooled_json(const object_type& type, const nlohmann::json& json)
	{
		return detail::get_pooled_json_int<NamedObject>(type, json);
	}
}
