#pragma once

#include "DLLBuild.h"

#include <glm\glm.hpp>

class Application;
struct GLFWwindow;

class DLL_BUILD Input {
	friend Application;
public:
	//is a_Key currently down
	static bool isKeyDown(unsigned int a_Key);
	//was a_Key pressed last frame or held down for a few seconds
	static bool isKeyRepeated(unsigned int a_Key);
	//was a_Key pressed last frame
	static bool wasKeyPressed(unsigned int a_Key);
	//was mouse button pressed this frame?
	static bool wasMousePressed(unsigned int a_MouseButton);
	//is a_MouseButtons down?
	static bool isMouseDown(unsigned int a_MouseButton);

	//sets a_MouseButton to be set to GLFW_RELEASE
	//does not stop the call when button is released
	static void removeMouseButton(unsigned int a_MouseButton);
	//sets a_Key button to be set to GLFW_RELEASE
	//does not stop the call when key is released
	static void removeKeyButton(unsigned int a_Key);
	//sets a_MouseButton to be set to GLFW_PRESSED
	//this makes the program think that the button was presed
	static void addMouseButton(unsigned int a_MouseButton);
	//sets a_Key button to be set to GLFW_PRESSED
	//does not stop the call when key is released
	static void addKeyButton(unsigned int a_Key);


	//resets mouse delta and position
	static void resetMouseDelta();

	static glm::vec2 getMousePos();
	static float getMouseX();
	static float getMouseY();
	static glm::vec2 getMouseDelta();
	static float getMouseDeltaX();
	static float getMouseDeltaY();
	static float getMouseScrollY();
protected:
	//called by glfw when a key is pressed
	static void keyCallback(GLFWwindow* a_Window, int a_Key, int a_ScanCode, int a_Action, int a_Mods);
	//called by glfw when mouse is moved
	static void cursorPositionCallback(GLFWwindow* a_Window, double a_XPos, double a_YPos);
	//called by glfw when a mouse button is pressed
	static void mouseButtonCallback(GLFWwindow* a_Window, int a_Button, int a_Action, int a_Mods);
	//called by glfw when mouse is scrolled or there is a touch pad gesture
	static void scrollCallback(GLFWwindow* a_Window, double a_X, double a_Y);
	//called by glfw when a character is pressed on the keyboard
	static void charCallback(GLFWwindow* a_Window, unsigned int a_Letter);

	//updates data that is frame dependent
	//eg: wasKeyPressed();
	static void update();
private:


};

