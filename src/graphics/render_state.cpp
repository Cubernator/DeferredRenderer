#include "graphics/render_state.hpp"
#include "util/json_utils.hpp"

json_interpreter<render_state> render_state::s_properties({
	{ "cull",			&render_state::getCull },
	{ "depthTest",		&render_state::getDepthTest },
	{ "depthWrite",		&render_state::getDepthWrite },
	{ "depthOffset",	&render_state::getDepthOffset },
	{ "blend",			&render_state::getBlend }
});

keyword_helper<render_state::cull_mode> render_state::s_cullModes({
	{ "back",			cull_back },
	{ "front",			cull_front },
	{ "frontAndBack",	cull_front_and_back }
});

keyword_helper<render_state::compare_function> render_state::s_compFunctions({
	{ "never",		cmp_never },
	{ "less",		cmp_less },
	{ "equal",		cmp_equal },
	{ "lequal",		cmp_lequal },
	{ "greater",	cmp_greater },
	{ "notEqual",	cmp_not_equal },
	{ "gequal",		cmp_gequal },
	{ "always",		cmp_always }
});

keyword_helper<render_state::blend_function> render_state::s_blendFuncs({
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

keyword_helper<render_state::blend_equation> render_state::s_blendEqtns({
	{ "add",				ble_add },
	{ "subtract",			ble_subtract },
	{ "reverseSubtract",	ble_reverse_subtract },
	{ "min",				ble_min },
	{ "max",				ble_max },
});


render_state::render_state()
	: cullEnable(true), cullMode(cull_back),
	depthWriteEnable(true),
	depthOffsetFactor(0.0f), depthOffsetUnits(0.0f),
	depthTestEnable(true), depthTestFunction(cmp_less),
	blendEnable(false), blendSourceFunction(blf_one), blendDestFunction(blf_zero), blendSourceAlphaFunction(blf_one), blendDestAlphaFunction(blf_zero),
	blendEquation(ble_add), blendEquationAlpha(ble_add), blendColor(0.0f, 0.0f, 0.0f, 0.0f) { }

void render_state::apply(bool cull, bool depthWrite, bool depthOffset, bool depthTest, bool blend) const
{
	if (cull) applyCulling();
	if (depthWrite) applyDepthWriting();
	if (depthOffset) applyDepthOffset();
	if (depthTest) applyDepthTest();
	if (blend) applyBlending();
}

void render_state::applyCulling() const
{
	if (cullEnable) {
		glEnable(GL_CULL_FACE);
		glCullFace(cullMode);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

void render_state::applyDepthWriting() const
{
	glDepthMask(depthWriteEnable ? GL_TRUE : GL_FALSE);
}

void render_state::applyDepthOffset() const
{
	glPolygonOffset(depthOffsetFactor, depthOffsetUnits);
}

void render_state::applyDepthTest() const
{
	if (depthTestEnable) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(depthTestFunction);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void render_state::applyBlending() const
{
	if (blendEnable) {
		glEnable(GL_BLEND);
		glBlendColor(blendColor.r, blendColor.g, blendColor.b, blendColor.a);
		glBlendEquationSeparate(blendEquation, blendEquationAlpha);
		glBlendFuncSeparate(blendSourceFunction, blendDestFunction, blendSourceAlphaFunction, blendDestAlphaFunction);
	} else {
		glDisable(GL_BLEND);
	}
}

void render_state::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void render_state::getCull(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		cullEnable = json.get<bool>();
	} else if (json.is_string()) {
		cullEnable = s_cullModes.get(json, cullMode);
	}
}

void render_state::getDepthTest(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		depthTestEnable = json.get<bool>();
	} else if (json.is_string()) {
		depthTestEnable = s_compFunctions.get(json, depthTestFunction);
	}
}

void render_state::getDepthWrite(const nlohmann::json& json)
{
	if (json.is_boolean()) {
		depthWriteEnable = json.get<bool>();
	}
}

void render_state::getDepthOffset(const nlohmann::json& json)
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

void render_state::getBlend(const nlohmann::json& json)
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

