#ifndef INPUT_INPUT_HPP
#define INPUT_INPUT_HPP

#include <unordered_map>

#include "util/singleton.hpp"
#include "glm.hpp"
#include "keys.hpp"

struct GLFWwindow;

namespace hexeract
{
	namespace input
	{
		class Input : public singleton<Input>
		{
		public:
			explicit Input(GLFWwindow* window);

			bool getKey(key key);
			bool getKeyPressed(key key);
			bool getKeyReleased(key key);

			bool getMouseButton(mbutton mbutton);
			bool getMouseButtonPressed(mbutton mbutton);
			bool getMouseButtonReleased(mbutton mbutton);

			bool isCursorLocked() const;
			void setCursorLocked(bool val);

			float cursorX() const { return m_cursorX; }
			float cursorY() const { return m_cursorY; }
			glm::vec2 cursorPos() const;

			float cursorDeltaX() const { return m_cursorDeltaX; }
			float cursorDeltaY() const { return m_cursorDeltaY; }
			glm::vec2 cursorDelta() const;

			void update();

		private:
			struct button
			{
				bool down;
				unsigned int lastAction;

				button() : down(false), lastAction(0) { }
			};

			using button_map = std::unordered_map<int, button>;

			GLFWwindow* m_window;

			button_map m_keys;
			button_map m_mbuttons;
			unsigned int m_frame;

			float m_cursorX, m_cursorY;
			float m_cursorDeltaX, m_cursorDeltaY;

			void onKey(int key, int action);
			void onMouseButton(int mbutton, int action);

			static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				instance()->onKey(key, action);
			}

			static void mouseButtonCallback(GLFWwindow* window, int mbutton, int action, int mods)
			{
				instance()->onMouseButton(mbutton, action);
			}
		};
	}
}

#endif // INPUT_INPUT_HPP