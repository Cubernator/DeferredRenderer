#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>

#include "glm.hpp"
#include "keys.hpp"

class Input
{
public:
	Input(GLFWwindow* window);

	bool getKey(input_key key);
	bool getKeyPressed(input_key key);
	bool getKeyReleased(input_key key);

	bool getMouseButton(input_mbutton mbutton);
	bool getMouseButtonPressed(input_mbutton mbutton);
	bool getMouseButtonReleased(input_mbutton mbutton);

	bool isCursorLocked() const;
	void setCursorLocked(bool val);

	float getCursorX() const { return m_cursorX; }
	float getCursorY() const { return m_cursorY; }
	void getCursorPos(float& x, float& y) const;
	glm::vec2 getCursorPos() const;

	float getCursorDeltaX() const { return m_cursorDeltaX; }
	float getCursorDeltaY() const { return m_cursorDeltaY; }
	void getCursorDelta(float& x, float& y) const;
	glm::vec2 getCursorDelta() const;

	void update();

	static Input* instance() { return s_instance; }

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

	static Input* s_instance;

	void onKey(int key, int action);
	void onMouseButton(int mbutton, int action);

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		s_instance->onKey(key, action);
	}

	static void mouseButtonCallback(GLFWwindow* window, int mbutton, int action, int mods)
	{
		s_instance->onMouseButton(mbutton, action);
	}
};

#endif // INPUT_HPP