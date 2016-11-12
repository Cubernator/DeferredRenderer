#ifndef RENDER_STATE_HPP
#define RENDER_STATE_HPP

#include "GL/glew.h"
#include "nlohmann/json.hpp"
#include "glm.hpp"
#include "util/keyword_helper.hpp"
#include "util/json_interpreter.hpp"
#include "util/json_initializable.hpp"

struct render_state : public json_initializable<render_state>
{
	enum cull_mode
	{
		cull_back			= GL_BACK,
		cull_front			= GL_FRONT,
		cull_front_and_back = GL_FRONT_AND_BACK
	};

	enum compare_function
	{
		cmp_never		= GL_NEVER,
		cmp_less		= GL_LESS,
		cmp_equal		= GL_EQUAL,
		cmp_lequal		= GL_LEQUAL,
		cmp_greater		= GL_GREATER,
		cmp_not_equal	= GL_NOTEQUAL,
		cmp_gequal		= GL_GEQUAL,
		cmp_always		= GL_ALWAYS
	};

	enum blend_function
	{
		blf_zero						= GL_ZERO,
		blf_one							= GL_ONE,
		blf_src_color					= GL_SRC_COLOR,
		blf_one_minus_src_color			= GL_ONE_MINUS_SRC_COLOR,
		blf_src_alpha					= GL_SRC_ALPHA,
		blf_one_minus_src_alpha			= GL_ONE_MINUS_SRC_ALPHA,
		blf_dst_alpha					= GL_DST_ALPHA,
		blf_one_minus_dst_alpha			= GL_ONE_MINUS_DST_ALPHA,
		blf_dst_color					= GL_DST_COLOR,
		blf_one_minus_dst_color			= GL_ONE_MINUS_DST_COLOR,
		blf_src_alpha_saturate			= GL_SRC_ALPHA_SATURATE,
		blf_constant_color				= GL_CONSTANT_COLOR,
		blf_one_minus_constant_color	= GL_ONE_MINUS_CONSTANT_COLOR,
		blf_constant_alpha				= GL_CONSTANT_ALPHA,
		blf_one_minus_constant_alpha	= GL_ONE_MINUS_CONSTANT_ALPHA
	};

	enum blend_equation
	{
		ble_add					= GL_FUNC_ADD,
		ble_subtract			= GL_FUNC_SUBTRACT,
		ble_reverse_subtract	= GL_FUNC_REVERSE_SUBTRACT,
		ble_min					= GL_MIN,
		ble_max					= GL_MAX
	};

	// culling
	bool cullEnable;
	cull_mode cullMode;

	// depth writing
	bool depthWriteEnable;

	// depth offset
	float depthOffsetFactor;
	float depthOffsetUnits;

	// depth test
	bool depthTestEnable;
	compare_function depthTestFunction;

	// blending
	bool blendEnable;
	blend_function blendSourceFunction;
	blend_function blendDestFunction;
	blend_function blendSourceAlphaFunction;
	blend_function blendDestAlphaFunction;
	blend_equation blendEquation;
	blend_equation blendEquationAlpha;
	glm::vec4 blendColor;

	render_state();

	void apply(
		bool cull = true,
		bool depthWrite = true,
		bool depthOffset = true,
		bool depthTest = true,
		bool blend = true
	) const;

	void applyCulling() const;
	void applyDepthWriting() const;
	void applyDepthOffset() const;
	void applyDepthTest() const;
	void applyBlending() const;

private:
	static json_interpreter<render_state> s_properties;

	static keyword_helper<cull_mode> s_cullModes;
	static keyword_helper<compare_function> s_compFunctions;
	static keyword_helper<blend_function> s_blendFuncs;
	static keyword_helper<blend_equation> s_blendEqtns;


	void apply_json_impl(const nlohmann::json& json);

	void getCull(const nlohmann::json& json);
	void getDepthTest(const nlohmann::json& json);
	void getDepthWrite(const nlohmann::json& json);
	void getDepthOffset(const nlohmann::json& json);
	void getBlend(const nlohmann::json& json);

	friend struct json_initializable<render_state>;
};

#endif // RENDER_STATE_HPP