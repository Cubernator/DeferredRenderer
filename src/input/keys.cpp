#include "keys.hpp"
#include "keyword_helper.hpp"

namespace hexeract
{
	namespace input
	{
		namespace
		{
			keyword_helper<key> g_keys{
				{ "space", key_space },
				{ "apostrophe", key_apostrophe },
				{ "comma", key_comma },
				{ "minus", key_minus },
				{ "period", key_period },
				{ "slash", key_slash },
				{ "0", key_0 },
				{ "1", key_1 },
				{ "2", key_2 },
				{ "3", key_3 },
				{ "4", key_4 },
				{ "5", key_5 },
				{ "6", key_6 },
				{ "7", key_7 },
				{ "8", key_8 },
				{ "9", key_9 },
				{ "semicolon", key_semicolon },
				{ "equal", key_equal },
				{ "a", key_a },
				{ "b", key_b },
				{ "c", key_c },
				{ "d", key_d },
				{ "e", key_e },
				{ "f", key_f },
				{ "g", key_g },
				{ "h", key_h },
				{ "i", key_i },
				{ "j", key_j },
				{ "k", key_k },
				{ "l", key_l },
				{ "m", key_m },
				{ "n", key_n },
				{ "o", key_o },
				{ "p", key_p },
				{ "q", key_q },
				{ "r", key_r },
				{ "s", key_s },
				{ "t", key_t },
				{ "u", key_u },
				{ "v", key_v },
				{ "w", key_w },
				{ "x", key_x },
				{ "y", key_y },
				{ "z", key_z },
				{ "leftBracket", key_left_bracket },
				{ "backslash", key_backslash },
				{ "rightBracket", key_right_bracket },
				{ "graveAccent", key_grave_accent },
				{ "world1", key_world_1 },
				{ "world2", key_world_2 },
				{ "escape", key_escape },
				{ "enter", key_enter },
				{ "tab", key_tab },
				{ "backspace", key_backspace },
				{ "insert", key_insert },
				{ "delete", key_delete },
				{ "right", key_right },
				{ "left", key_left },
				{ "down", key_down },
				{ "up", key_up },
				{ "pageUp", key_page_up },
				{ "pageDown", key_page_down },
				{ "home", key_home },
				{ "end", key_end },
				{ "capsLock", key_caps_lock },
				{ "scrollLock", key_scroll_lock },
				{ "numLock", key_num_lock },
				{ "printScreen", key_print_screen },
				{ "pause", key_pause },
				{ "f1", key_f1 },
				{ "f2", key_f2 },
				{ "f3", key_f3 },
				{ "f4", key_f4 },
				{ "f5", key_f5 },
				{ "f6", key_f6 },
				{ "f7", key_f7 },
				{ "f8", key_f8 },
				{ "f9", key_f9 },
				{ "f10", key_f10 },
				{ "f11", key_f11 },
				{ "f12", key_f12 },
				{ "f13", key_f13 },
				{ "f14", key_f14 },
				{ "f15", key_f15 },
				{ "f16", key_f16 },
				{ "f17", key_f17 },
				{ "f18", key_f18 },
				{ "f19", key_f19 },
				{ "f20", key_f20 },
				{ "f21", key_f21 },
				{ "f22", key_f22 },
				{ "f23", key_f23 },
				{ "f24", key_f24 },
				{ "f25", key_f25 },
				{ "numpad0", key_numpad_0 },
				{ "numpad1", key_numpad_1 },
				{ "numpad2", key_numpad_2 },
				{ "numpad3", key_numpad_3 },
				{ "numpad4", key_numpad_4 },
				{ "numpad5", key_numpad_5 },
				{ "numpad6", key_numpad_6 },
				{ "numpad7", key_numpad_7 },
				{ "numpad8", key_numpad_8 },
				{ "numpad9", key_numpad_9 },
				{ "numpadDecimal", key_numpad_decimal },
				{ "numpadDivide", key_numpad_divide },
				{ "numpadMultiply", key_numpad_multiply },
				{ "numpadSubtract", key_numpad_subtract },
				{ "numpadAdd", key_numpad_add },
				{ "numpadEnter", key_numpad_enter },
				{ "numpadEqual", key_numpad_equal },
				{ "leftShift", key_left_shift },
				{ "leftControl", key_left_control },
				{ "leftAlt", key_left_alt },
				{ "leftSuper", key_left_super },
				{ "rightShift", key_right_shift },
				{ "rightControl", key_right_control },
				{ "rightAlt", key_right_alt },
				{ "rightSuper", key_right_super },
				{ "menu", key_menu }
			};

			keyword_helper<mbutton> g_mbuttons{
				{ "mbutton1", mbutton_1 },
				{ "mbutton2", mbutton_2 },
				{ "mbutton3", mbutton_3 },
				{ "mbutton4", mbutton_4 },
				{ "mbutton5", mbutton_5 },
				{ "mbutton6", mbutton_6 },
				{ "mbutton7", mbutton_7 },
				{ "mbutton8", mbutton_8 },
				{ "left", mbutton_left },
				{ "right", mbutton_right },
				{ "middle", mbutton_middle },
			};
		}

		key string_to_key(const std::string& s)
		{
			key result = key_unknown;
			g_keys.get(s, result);
			return result;
		}

		mbutton string_to_mbutton(const std::string& s)
		{
			mbutton result = mbutton_unknown;
			g_mbuttons.get(s, result);
			return result;
		}
	}
}