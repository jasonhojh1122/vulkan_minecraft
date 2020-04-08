#pragma once

#include <glfw/glfw3.h>
#include "Camera.h"

class UserInputManager {
public:
	UserInputManager(Camera* cam) : camera(cam) {};
	void cursorManager(double xOffset, double yOffset);
	void scrollManager(double yOffset);
	void keyPressManager(GLFWwindow* window, double deltaTime);

private:
	Camera* camera;
};

void UserInputManager::cursorManager(double xOffset, double yOffset) {
	camera->processMouseMovement(xOffset, yOffset);
}

void UserInputManager::scrollManager(double yOffset) {
	camera->processMouseScroll(yOffset);
}

void UserInputManager::keyPressManager(GLFWwindow* window, double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(RIGHT, deltaTime);
}
