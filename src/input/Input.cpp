#include "Input.hpp"
#include "scripting/class_registry.hpp"

#include "GLFW/glfw3.h"

namespace hexeract
{
	namespace input
	{
		Input::Input(GLFWwindow* window) : m_window(window), m_frame(0), m_cursorX(0.0f), m_cursorY(0.0f), m_cursorDeltaX(0.0f), m_cursorDeltaY(0.0f)
		{
			glfwSetKeyCallback(m_window, Input::keyCallback);
			glfwSetMouseButtonCallback(m_window, Input::mouseButtonCallback);
		}

		bool Input::getKey(key key)
		{
			return m_keys[int(key)].down;
		}

		bool Input::getKeyPressed(key key)
		{
			button& k = m_keys[int(key)];
			return k.down && (k.lastAction == m_frame);
		}

		bool Input::getKeyReleased(key key)
		{
			button& k = m_keys[int(key)];
			return (!k.down) && (k.lastAction == m_frame);
		}

		bool Input::getMouseButton(mbutton mbutton)
		{
			return m_mbuttons[int(mbutton)].down;
		}

		bool Input::getMouseButtonPressed(mbutton mbutton)
		{
			button& mb = m_mbuttons[int(mbutton)];
			return mb.down && (mb.lastAction == m_frame);
		}

		bool Input::getMouseButtonReleased(mbutton mbutton)
		{
			button& mb = m_mbuttons[int(mbutton)];
			return (!mb.down) && (mb.lastAction == m_frame);
		}

		bool Input::isCursorLocked() const
		{
			return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
		}

		void Input::setCursorLocked(bool val)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, val ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}

		glm::vec2 Input::cursorPos() const
		{
			return glm::vec2(m_cursorX, m_cursorY);
		}

		glm::vec2 Input::cursorDelta() const
		{
			return glm::vec2(m_cursorDeltaX, m_cursorDeltaY);
		}

		void Input::update()
		{
			++m_frame;

			glfwPollEvents();

			double cx, cy;
			glfwGetCursorPos(m_window, &cx, &cy);
			float newX = float(cx), newY = float(cy);

			m_cursorDeltaX = newX - m_cursorX;
			m_cursorDeltaY = newY - m_cursorY;

			m_cursorX = newX;
			m_cursorY = newY;
		}

		void Input::onKey(int key, int action)
		{
			button& k = m_keys[key];
			if (action != GLFW_REPEAT) {
				k.down = action == GLFW_PRESS;
				k.lastAction = m_frame;
			}
		}

		void Input::onMouseButton(int mbutton, int action)
		{
			button& mb = m_mbuttons[mbutton];
			mb.down = action == GLFW_PRESS;
			mb.lastAction = m_frame;
		}


		key check_key(lua_State* L, int arg)
		{
			int t = lua_type(L, arg);
			switch (t) {
			case LUA_TNUMBER:
			{
				auto i = lua_tointeger(L, arg);
				return static_cast<key>(i);
			}
			case LUA_TSTRING:
			{
				std::string s = lua_tostring(L, arg);
				key k = string_to_key(s);
				if (k == key_unknown) {
					luaL_argerror(L, 1, ("unknown key: " + s).c_str());
				}
				return k;
			}
			default:
			{
				luaL_argerror(L, arg, "expected string or integer");
			}
			}
			return key_unknown;
		}

		mbutton check_mbutton(lua_State* L, int arg)
		{
			int t = lua_type(L, arg);
			switch (t) {
			case LUA_TNUMBER:
			{
				auto i = lua_tointeger(L, arg);
				return static_cast<mbutton>(i);
			}
			case LUA_TSTRING:
			{
				std::string s = lua_tostring(L, arg);
				mbutton b = string_to_mbutton(s);
				if (b == mbutton_unknown) {
					luaL_argerror(L, arg, ("unknown mouse button: " + s).c_str());
				}
				return b;
			}
			default:
			{
				luaL_argerror(L, arg, "expected string or integer");
			}
			}
			return mbutton_unknown;
		}

		SCRIPTING_REGISTER_STATIC_CLASS(Input);

		SCRIPTING_DEFINE_METHOD(Input, getKey)
		{
			scripting::push_value(L, Input::instance()->getKey(check_key(L, 1)));
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Input, getKeyPressed)
		{
			scripting::push_value(L, Input::instance()->getKeyPressed(check_key(L, 1)));
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Input, getKeyReleased)
		{
			scripting::push_value(L, Input::instance()->getKeyReleased(check_key(L, 1)));
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Input, getMouseButton)
		{
			scripting::push_value(L, Input::instance()->getMouseButton(check_mbutton(L, 1)));
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Input, getMouseButtonPressed)
		{
			scripting::push_value(L, Input::instance()->getMouseButtonPressed(check_mbutton(L, 1)));
			return 1;
		}

		SCRIPTING_DEFINE_METHOD(Input, getMouseButtonReleased)
		{
			scripting::push_value(L, Input::instance()->getMouseButtonReleased(check_mbutton(L, 1)));
			return 1;
		}

		SCRIPTING_AUTO_MODULE_METHOD(Input, isCursorLocked);
		SCRIPTING_AUTO_MODULE_METHOD(Input, setCursorLocked);

		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorX);
		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorY);
		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorPos);

		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorDeltaX);
		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorDeltaY);
		SCRIPTING_AUTO_MODULE_METHOD(Input, cursorDelta);
	}
}