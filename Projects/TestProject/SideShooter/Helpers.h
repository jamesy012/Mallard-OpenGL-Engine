#pragma once

#include <glm\glm.hpp>
#include "Camera.h"
#include "Window.h"
#include <cmath>

static glm::vec2 worldToScreenSpace(glm::vec3 a_Position, Camera* a_Camera) {
	glm::vec4 point = a_Camera->getProjectionMatrix() * (a_Camera->getViewMatrix() * glm::vec4(a_Position, 1));
	glm::vec3 screenSapce = glm::vec3(point.x, point.y, point.z) / point.w;
	Window* mainW = Window::getMainWindow();

	glm::vec2 screenSpace;
	screenSpace.x = round(((screenSapce.x + 1) / 2.0f) * mainW->getWindowWidth());
	screenSpace.y = round(((screenSapce.y + 1) / 2.0f) * mainW->getWindowHeight());
	return screenSpace;
}
static bool isOnScreen(glm::vec3 a_Position, Camera* a_Camera) {
	glm::vec2 pos = worldToScreenSpace(a_Position, a_Camera);
	if (pos.x < 0 || pos.y < 0) {
		return false;
	}
	Window* mainW = Window::getMainWindow();
	if (pos.x > mainW->getWindowWidth() || pos.y > mainW->getWindowHeight()) {
		return false;
	}
	return true;
}

static int howFarOffScreen(glm::vec3 a_Position, Camera* a_Camera) {
	glm::vec2 pos = worldToScreenSpace(a_Position, a_Camera);
	Window* mainW = Window::getMainWindow();
	int min = 9999;
	min = (int)std::fmin(min, pos.x);
	min = (int)std::fmin(min, pos.y);
	min = (int)std::fmin(min, mainW->getWindowWidth()-pos.x);
	min = (int)std::fmin(min, mainW->getWindowHeight()-pos.y);
	return min;
}