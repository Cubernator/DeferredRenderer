#include "RenderState.hpp"
#include "util/json_utils.hpp"

json_interpreter<RenderState> RenderState::s_properties({
	{ "cull",			&RenderState::extractCull },
	{ "depthTest",		&RenderState::extractDepthTest },
	{ "depthWrite",		&RenderState::extractDepthWrite },
	{ "depthOffset",	&RenderState::extractDepthOffset },
	{ "blend",			&RenderState::extractBlend }
});

keyword_helper<RenderState::cull_mode> RenderState::s_cullModes({
	{ "back",			cull_back },
	{ "front",			cull_front },
	{ "frontAndBack",	cull_front_and_back }
});

keyword_helper<RenderState::compare_function> RenderState::s_compFunctions({
	{ "never",		cmp_never },
	{ "less",		cmp_less },
	{ "equal",		cmp_equal },
	{ "lequal",		cmp_lequal },
	{ "greater",	cmp_greater },
	{ "notEqual",	cmp_not_equal },
	{ "gequal",		cmp_gequal },
	{ "always",		cmp_always }
});

keyword_helper<RenderState::blend_function> RenderState::s_blendFuncs({
	{ "zero",					blf_zero },
	{ "one",					blf_one },
	{ "srcColor",				blf_src_color },
	{ "oneMinusSrcColor",		blf_one_minus_src_color },
	{ "srcAlpha",				blf_src_alpha },
	{ "oneMinusSrcAlpha",		blf_one_minus_src_alpha },
	{ "dstAlpha",				blf_dst_alpha },
	{ "oneMinusDstAlpha",		blf_one_minus_dst_alpha },
	{ "dstColor",				blf_dst_color },
	{ "oneMinusDstColor",		blf_one_minus_dst_color },
	{ "srcAlphaSaturate",		blf_src_alpha_saturate },
	{ "constantColor",			blf_constant_color },
	{ "oneMinusConstantColor",	blf_one_minus_constant_color },
	{ "constantAlpha",			blf_constant_alpha },
	{ "oneMinusConstantAlpha",	blf_one_minus_constant_alpha }
});

keyword_helper<RenderState::blend_equation> RenderState::s_blendEqtns({
	{ "add",				ble_add },
	{ "subtract",			ble_subtract },
	{ "reverseSubtract",	ble_reverse_subtract },
	{ "min",				ble_min },
	{ "max",				ble_max },
});


RenderState::RenderState()
	: cullEnable(true), cullMode(cull_back),
	depthWriteEnable(true),
	depthOffsetFactor(0.0f), depthOffsetUnits(0.0f),
	depthTestEnable(true), depthTestFunction(cmp_less),
	blendEnable(false), blendSourceFunction(blf_one), blendDestFunction(blf_zero), blendSourceAlphaFunction(blf_one), blendDestAlphaFunction(blf_zero),
	blendEquation(ble_add), blendEquationAlpha(ble_add), blendColor(0.0f, 0.0f, 0.0f, 0.0f) { }

void RenderState::apply() const
{
	applyCulling();
	applyDepthWriting();
	applyDepthOffset();
	applyDepthTest();
	applyBlending();
}

void RenderState::differentialApply(const RenderState& other) const
{
	applyCullingDiff(other);
	applyDepthWriteDiff(other);
	applyDepthOffsetDiff(other);
	applyDepthTestDiff(other);
	applyBlendDiff(other);
}

void RenderState::applyCulling() const
{
	setEnabled(GL_CULL_FACE, cullEnable);
	if (cullEnable)
		applyCullMode();
}

void RenderState::applyDepthTest() const
{
	setEnabled(GL_DEPTH_TEST, depthTestEnable);
	if (depthTestEnable)
		applyDepthTestFunc();
}

void RenderState::applyBlending() const
{
	setEnabled(GL_BLEND, blendEnable);
	if (blendEnable) {
		applyBlendColor();
		applyBlendEquation();
		applyBlendFunction();
	}
}


void RenderState::applyCullingDiff(const RenderState& other) const
{
	if (other.cullEnable != cullEnable)
		setEnabled(GL_CULL_FACE, cullEnable);

	if (other.cullMode != cullMode)
		applyCullMode();
}

void RenderState::applyDepthWriteDiff(const RenderState& other) const
{
	if (other.depthWriteEnable != depthWriteEnable)
		applyDepthWriting();
}

void RenderState::applyDepthOffsetDiff(const RenderState& other) const
{
	if ((other.depthOffsetFactor != depthOffsetFactor) || (other.depthOffsetUnits != depthOffsetUnits))
		applyDepthOffset();
}

void RenderState::applyDepthTestDiff(const RenderState& other) const
{
	if (other.depthTestEnable != depthTestEnable)
		setEnabled(GL_DEPTH_TEST, depthTestEnable);

	if (other.depthTestFunction != depthTestFunction)
		applyDepthTestFunc();
}

void RenderState::applyBlendDiff(const RenderState& other) const
{
	if (other.blendEnable != blendEnable)
		setEnabled(GL_BLEND, blendEnable);

	if (other.blendColor != blendColor)
		applyBlendColor();

	if ((other.blendEquation != blendEquation) || (other.blendEquationAlpha != blendEquationAlpha))
		applyBlendEquation();

	if ((other.blendSourceFunction != blendSourceFunction)
		|| (other.blendSourceAlphaFunction != blendSourceAlphaFunction)
		|| (other.blendDestFunction != blendDestFunction)
		|| (other.blendDestAlphaFunction != blendDestAlphaFunction))
		applyBlendFunction();
}

void RenderState::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void RenderState::extractCull(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		cullEnable = json.get<bool>();
	} else if (json.is_string()) {
		cullEnable = s_cullModes.get(json, cullMode);
	}
}

void RenderState::extractDepthTest(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		depthTestEnable = json.get<bool>();
	} else if (json.is_string()) {
		depthTestEnable = s_compFunctions.get(json, depthTestFunction);
	}
}

void RenderState::extractDepthWrite(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		depthWriteEnable = json.get<bool>();
	}
}

void RenderState::extractDepthOffset(const nlohmann::json& json)
{
	if (json.is_array()) {
		unsigned int s = json.size();
		if (s > 0) {
			auto& j0 = json.at(0);
			if (j0.is_number()) depthOffsetFactor = j0.get<float>();
		}
		if (s > 1) {
			auto& j1 = json.at(1);
			if (j1.is_number()) depthOffsetUnits = j1.get<float>();
		}
	}
}

void RenderState::extractBlend(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		blendEnable = json.get<bool>();
	} else if (json.is_object()) {
		std::string src, dst, srca, dsta, eqn, eqna, col;

		bool s  = get_value<std::string>(json, "source", src);
		bool d  = get_value<std::string>(json, "dest", dst);
		bool sa = get_value<std::string>(json, "sourceAlpha", srca);
		bool da = get_value<std::string>(json, "destAlpha", dsta);
		bool e  = get_value<std::string>(json, "equation", eqn);
		bool ea = get_value<std::string>(json, "equationAlpha", eqna);
		bool c  = get_value<std::string>(json, "color", col);

		if (s) {
			s_blendFuncs.get(src, blendSourceFunction);
			if (!sa) blendSourceAlphaFunction = blendSourceFunction;
		}
		if (sa) s_blendFuncs.get(srca, blendSourceAlphaFunction);

		if (d) {
			s_blendFuncs.get(dst, blendDestFunction);
			if (!da) blendDestAlphaFunction = blendDestFunction;
		}
		if (da) s_blendFuncs.get(dsta, blendDestAlphaFunction);

		if (e) {
			s_blendEqtns.get(eqn, blendEquation);
			if (!ea) blendEquationAlpha = blendEquation;
		}
		if (ea) s_blendEqtns.get(eqna, blendEquationAlpha);

		if (c) blendColor = parse_color(col);

		blendEnable = true;
	}
}

