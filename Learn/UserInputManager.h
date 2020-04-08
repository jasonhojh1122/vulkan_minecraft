#pragma once

#include <glfw/glfw3.h>
#include "Camera.h"

class UserInputManager {
public:
	UserInputManager(Camera* cam) : camera(cam) {};
	void cursorManager(GLFWwindow* window, double xOffset, double yOffset);
	void scrollManager(double yOffset);
	void keyPressManager(GLFWwindow* window, double deltaTime);
	void mousceButtonManager(GLFWwindow* window, int button, int action);

private:
	Camera* camera;
	double lastX, lastY;
	bool firstMouse = true;
	bool mouseLeftButtonIsClick = false;
};

void UserInputManager::cursorManager(GLFWwindow* window, double xpos, double ypos) {
	if (mouseLeftButtonIsClick) {
		float xoffset = xpos - lastX;
		float yoffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;
		camera->processMouseMovement(xoffset, yoffset);
		std::cout << xpos << ' ' << ypos << std::endl;
	}
}

void UserInputManager::scrollManager(double yOffset) {
	camera->processMouseScroll(yOffset);
}

void UserInputManager::keyPressManager(GLFWwindow* window, double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->processKeyboard(CAM_FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(CAM_BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(CAM_LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(CAM_RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera->processKeyboard(CAM_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera->processKeyboard(CAM_DOWN, deltaTime);

}

void UserInputManager::mousceButtonManager(GLFWwindow* window, int button, int action) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseLeftButtonIsClick = true;
		glfwGetCursorPos(window, &lastX, &lastY);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseLeftButtonIsClick = false;
	}
}