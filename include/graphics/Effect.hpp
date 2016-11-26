#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "util/import.hpp"
#include "util/keyword_helper.hpp"
#include "util/json_interpreter.hpp"
#include "util/json_initializable.hpp"
#include "shader/shader_property.hpp"
#include "render_state.hpp"

#include "boost/multi_index_container.hpp"

namespace mi = boost::multi_index;

class ShaderProgram;
class Material;

class Effect : public json_initializable<Effect>
{
public:
	enum light_mode
	{
		light_forward_base,
		light_forward_add,
		light_deferred,
		light_shadow_cast,
		light_deferred_ambient,
		light_deferred_light
	};

	enum render_queue
	{
		queue_background = 0,
		queue_geometry = 1000,
		queue_alpha_test = 2000,
		queue_transparent = 3000,
		queue_effect = 4000
	};

	enum render_type
	{
		type_opaque,
		type_cutout,
		type_transparent
	};

	struct pass : public json_initializable<pass>
	{
		std::string name;
		light_mode mode;
		render_state state;
		ShaderProgram* program;

		pass();

	private:
		static json_interpreter<pass> s_properties;

		void apply_json_impl(const nlohmann::json& json);

		void getName(const nlohmann::json& json);
		void getLightMode(const nlohmann::json& json);
		void getState(const nlohmann::json& json);
		void getProgram(const nlohmann::json& json);

		friend struct json_initializable<pass>;
	};

	Effect();

	render_type getRenderType() const { return m_renderType; }

	const pass* getPass(light_mode mode);
	const pass* getPass(const std::string& name);

	void applyProperties(const pass* p, const Material* m);

	shader_property_map::const_iterator begin_properties() const { return m_properties.cbegin(); }
	shader_property_map::const_iterator end_properties() const { return m_properties.cend(); }

private:
	struct by_mode { };
	struct by_name { };

	struct pass_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_mode>, mi::member<pass, light_mode, &pass::mode>>,
		mi::hashed_unique<mi::tag<by_name>, mi::member<pass, std::string, &pass::name>>
	> { };

	using pass_container = mi::multi_index_container<
		pass,
		pass_container_indices
	>;

	render_type m_renderType;
	int m_queuePriority;

	shader_property_map m_properties;
	pass_container m_passes;

	static json_interpreter<Effect> s_properties;

	static keyword_helper<render_queue> s_renderQueues;
	static keyword_helper<render_type> s_renderTypes;
	static keyword_helper<light_mode> s_lightModes;


	void apply_json_impl(const nlohmann::json& json);

	void extractRenderQueue(const nlohmann::json& json);
	void extractRenderType(const nlohmann::json& json);
	void extractProperties(const nlohmann::json& json);
	void extractPasses(const nlohmann::json& json);

	void addProperty(const nlohmann::json& json);
	void addPass(const nlohmann::json& json);

	friend struct json_initializable<Effect>;
};

#endif // EFFECT_HPP