#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "util/import.hpp"
#include "util/keyword_helper.hpp"
#include "util/json_interpreter.hpp"
#include "util/json_initializable.hpp"
#include "shader/shader_property.hpp"
#include "RenderState.hpp"

#include "boost/multi_index_container.hpp"

namespace mi = boost::multi_index;

class ShaderProgram;
class Material;

enum light_mode
{
	light_forward_base,
	light_forward_add,
	light_deferred,
	light_shadow_cast
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

class Pass : public json_initializable<Pass>
{
public:
	std::string name;
	light_mode mode;
	RenderState state;
	ShaderProgram* program;

	Pass();

private:
	static json_interpreter<Pass> s_properties;

	void apply_json_impl(const nlohmann::json& json);

	void extractState(const nlohmann::json& json);
	void extractProgram(const nlohmann::json& json);

	friend struct json_initializable<Pass>;
};

class Effect : public json_initializable<Effect>
{
public:
	Effect();

	render_type getRenderType() const { return m_renderType; }
	void setRenderType(render_type val) { m_renderType = val; }

	int getQueuePriority() const { return m_queuePriority; }
	void setQueuePriority(int val) { m_queuePriority = val; }

	const Pass* getPass(light_mode mode) const;
	const Pass* getPass(const std::string& name) const;

	void applyProperties(const ShaderProgram* p, const Material* m) const;

	shader_property_map::const_iterator begin_properties() const { return m_properties.cbegin(); }
	shader_property_map::const_iterator end_properties() const { return m_properties.cend(); }

private:
	struct by_mode { };
	struct by_name { };

	struct pass_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_name>, mi::member<Pass, std::string, &Pass::name>>,
		mi::hashed_non_unique<mi::tag<by_mode>, mi::member<Pass, light_mode, &Pass::mode>>
	> { };

	using pass_container = mi::multi_index_container<
		Pass,
		pass_container_indices
	>;

	render_type m_renderType;
	int m_queuePriority;

	shader_property_map m_properties;
	pass_container m_passes;

	static json_interpreter<Effect> s_properties;


	void apply_json_impl(const nlohmann::json& json);

	void extractRenderQueue(const nlohmann::json& json);
	void extractProperties(const nlohmann::json& json);
	void extractPasses(const nlohmann::json& json);

	void addProperty(const nlohmann::json& json);
	void addPass(const nlohmann::json& json);

	friend struct json_initializable<Effect>;
};

#endif // EFFECT_HPP