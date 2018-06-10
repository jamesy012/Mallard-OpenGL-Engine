#include "Input.h"

#include <GLFW\glfw3.h>

#include <glm\glm.hpp>

#include <map>
#include <stdio.h> //printf


//keys which were pressed or released this frame, does not include repeated keys
//<key ID, was it interacted with> 
//the 2nd parameter defaults to false
static std::map<unsigned int,bool> m_KeysInteractedWithThisFrame;
//array of keys with last action used on them
//GLFW_KEY_LAST  constant holds the highest value of any named button.
static unsigned int m_KeysDown[GLFW_KEY_LAST];

//mouse buttons which were pressed or released this frame
//<Button ID, was it interacted with?> 
//the 2nd parameter defaults to false
static std::map<unsigned int, bool> m_MouseInteractedWithThisFrame;
//array of keys with last action used on them
//GLFW_MOUSE_BUTTON_LAST  constant holds the highest value of any named button.
static unsigned int m_MouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];

//mouse pos in window
static glm::vec2 m_MousePos;
//how much the mouse has moved since the last frame
static glm::vec2 m_MouseDelta;
//delta of the mouse scroll per frame
static glm::vec2 m_MouseScroll;

bool Input::isKeyDown(unsigned int a_Key) {
	return m_KeysDown[a_Key] == GLFW_PRESS || m_KeysDown[a_Key] == GLFW_REPEAT;
}

bool Input::isKeyRepeated(unsigned int a_Key) {
	//is the key down?
	if (isKeyDown(a_Key)) {
		//is key in m_KeysInteractedWithThisFrame
		return m_KeysInteractedWithThisFrame[a_Key];
	}
	//if not in either of those then the key was not pressed
	return false;
}

bool Input::wasKeyPressed(unsigned int a_Key) {
	//is the key down?
	if (m_KeysDown[a_Key] == GLFW_PRESS) {
		//is key in m_KeysInteractedWithThisFrame
		return m_KeysInteractedWithThisFrame[a_Key];
	}
	//if not in either of those then the key was not pressed
	return false;
}

bool Input::wasMousePressed(unsigned int a_MouseButton) {
	if (a_MouseButton > GLFW_MOUSE_BUTTON_LAST) {
		return false;
	}
	if (isMouseDown(a_MouseButton)) {
		return m_MouseInteractedWithThisFrame[a_MouseButton];
	}
	return false;
}

bool Input::isMouseDown(unsigned int a_MouseButton) {
	if (a_MouseButton > GLFW_MOUSE_BUTTON_LAST) {
		return false;
	}
	//return true if mouse button is pressed
	return m_MouseButtonsDown[a_MouseButton] == GLFW_PRESS;
}

void Input::removeMouseButton(unsigned int a_MouseButton) {
	m_MouseInteractedWithThisFrame[a_MouseButton] = false;
	m_MouseButtonsDown[a_MouseButton] = GLFW_RELEASE;
}

void Input::removeKeyButton(unsigned int a_Key) {
	m_KeysInteractedWithThisFrame[a_Key] = false;
	m_KeysDown[a_Key] = GLFW_RELEASE;
}

void Input::resetMouseDelta() {
	m_MousePos = glm::vec2(0, 0);
	m_MouseDelta = glm::vec2(0, 0);
}

glm::vec2 Input::getMousePos() {
	return m_MousePos;
}

float Input::getMouseX() {
	return m_MousePos.x;
}

float Input::getMouseY() {
	return m_MousePos.y;
}

glm::vec2 Input::getMouseDelta() {
	return m_MouseDelta;
}

float Input::getMouseDeltaX() {
	return m_MouseDelta.x;
}

float Input::getMouseDeltaY() {
	return m_MouseDelta.y;
}

float Input::getMouseScrollY() {
	return m_MouseScroll.y;
}

void Input::keyCallback(GLFWwindow * a_Window, int a_Key, int a_ScanCode, int a_Action, int a_Mods) {
	//printf("%i %i %i %i : %i\n", GLFW_RELEASE, GLFW_PRESS, GLFW_REPEAT, GLFW_KEY_UNKNOWN, a_Action);

	if (a_Action == GLFW_KEY_UNKNOWN) {
		return;
	}

	m_KeysInteractedWithThisFrame[a_Key] = true;
	m_KeysDown[a_Key] = a_Action;
}

void Input::cursorPositionCallback(GLFWwindow * a_Window, double a_XPos, double a_YPos) {
	glm::vec2 oldPos = m_MousePos;
	m_MousePos = glm::vec2(a_XPos, a_YPos);

	//todo: might need to reset MouseDelta after mouse has left the window
	//if not first frame, perhaps get a better test
	if (oldPos != glm::vec2(0, 0)) {
		m_MouseDelta += m_MousePos - oldPos;
	}
}

void Input::mouseButtonCallback(GLFWwindow * a_Window, int a_Button, int a_Action, int a_Mods) {
	//printf("Mouse Callback: %i\n", a_Button);

	//if the mouse is not press or release, this is not a action we want to respond to
	if (a_Action != GLFW_PRESS && a_Action != GLFW_RELEASE) {
		return;
	}
	
	m_MouseInteractedWithThisFrame[a_Button] = true;
	m_MouseButtonsDown[a_Button] = a_Action;
}

void Input::scrollCallback(GLFWwindow * a_Window, double a_X, double a_Y) {
	m_MouseScroll += glm::vec2(a_X, a_Y);
}

void Input::charCallback(GLFWwindow * a_Window, unsigned int a_Letter) {

}

void Input::update() {
	m_KeysInteractedWithThisFrame.clear();
	m_MouseInteractedWithThisFrame.clear();
	m_MouseDelta = m_MouseScroll = glm::vec2(0);
}
