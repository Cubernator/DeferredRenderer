#include "graphics/shader_property.hpp"

shader_property::converter_container shader_property::s_converters({
	make_converter<float>("float"),
	make_converter<glm::vec2>("vec2"),
	make_converter<glm::vec3>("vec3"),
	make_converter<glm::vec4>("vec4"),

	make_converter<int>("int"),
	make_converter<glm::ivec2>("ivec2"),
	make_converter<glm::ivec3>("ivec3"),
	make_converter<glm::ivec4>("ivec4"),

	make_converter<unsigned int>("uint"),
	make_converter<glm::uvec2>("uvec2"),
	make_converter<glm::uvec3>("uvec3"),
	make_converter<glm::uvec4>("uvec4"),

	make_converter<glm::mat4>("mat4"),
	make_converter<glm::mat3>("mat3"),
	make_converter<glm::mat4x3>("mat4x3")
});


void shader_property::assign_json(const nlohmann::json& json)
{
	assign_json(boost::type_erasure::typeid_of(value), json);
}

void shader_property::assign_json(const std::type_info& type, const nlohmann::json& json)
{
	auto& index_by_type = s_converters.get<by_type>();
	auto it = index_by_type.find(type);
	if (it != index_by_type.end()) {
		value = it->function(json);
	}
}

void shader_property::assign_json(const std::string& type_name, const nlohmann::json& json)
{
	auto& index_by_name = s_converters.get<by_name>();
	auto it = index_by_name.find(type_name);
	if (it != index_by_name.end()) {
		value = it->function(json);
	}
}

void shader_property::apply_json_impl(const nlohmann::json& json)
{
	auto nit = json.find("name");
	if (nit != json.end() && nit->is_string()) {
		name = nit->get<std::string>();
	}

	auto tit = json.find("type");
	auto dit = json.find("default");
	if (tit != json.end() && dit != json.end() && tit->is_string()) {
		assign_json(tit->get<std::string>(), *dit);
	}
}

