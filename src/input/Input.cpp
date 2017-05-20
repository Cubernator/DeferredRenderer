#include "Input.hpp"

#include "GLFW/glfw3.h"

Input* Input::s_instance(nullptr);

Input::Input(GLFWwindow* window) : m_window(window), m_frame(0), m_cursorX(0.0f), m_cursorY(0.0f), m_cursorDeltaX(0.0f), m_cursorDeltaY(0.0f)
{
	s_instance = this;

	glfwSetKeyCallback(m_window, Input::keyCallback);
	glfwSetMouseButtonCallback(m_window, Input::mouseButtonCallback);
}

bool Input::getKey(input_key key)
{
	return m_keys[int(key)].down;
}

bool Input::getKeyPressed(input_key key)
{
	button& k = m_keys[int(key)];
	return k.down && (k.lastAction == m_frame);
}

bool Input::getKeyReleased(input_key key)
{
	button& k = m_keys[int(key)];
	return (!k.down) && (k.lastAction == m_frame);
}

bool Input::getMouseButton(input_mbutton mbutton)
{
	return m_mbuttons[int(mbutton)].down;
}

bool Input::getMouseButtonPressed(input_mbutton mbutton)
{
	button& mb = m_mbuttons[int(mbutton)];
	return mb.down && (mb.lastAction == m_frame);
}

bool Input::getMouseButtonReleased(input_mbutton mbutton)
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

void Input::getCursorPos(float& x, float& y) const
{
	x = m_cursorX;
	y = m_cursorY;
}

glm::vec2 Input::getCursorPos() const
{
	return glm::vec2(m_cursorX, m_cursorY);
}

void Input::getCursorDelta(float& x, float& y) const
{
	x = m_cursorDeltaX;
	y = m_cursorDeltaY;
}

glm::vec2 Input::getCursorDelta() const
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

