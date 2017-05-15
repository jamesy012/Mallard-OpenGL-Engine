#include "Application.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

Application::Application() {
}


Application::~Application() {
}

float Application::getRandom() {
	glm::vec3 rand = glm::sphericalRand(5.0f);
	return rand.x;
}
