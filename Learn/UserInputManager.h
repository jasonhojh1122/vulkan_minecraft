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

	glm::vec4 getLightPos() { return lightPos; };


private:
	Camera* camera;
	double lastX, lastY;
	bool firstMouse = true;
	bool mouseLeftButtonIsClick = false;

	glm::vec4 lightPos = glm::vec4(0.0, 0.0, 8.0, 0.0);
	float dLightMovement = 0.1f;
};

void UserInputManager::cursorManager(GLFWwindow* window, double xpos, double ypos) {
	if (mouseLeftButtonIsClick) {
		float xoffset = xpos - lastX;
		float yoffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;
		camera->processMouseMovement(xoffset, yoffset);
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

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.x += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.x -= dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.y += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.y -= dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		lightPos.z += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		lightPos.z -= dLightMovement;

	std::cout << lightPos.x << ' ' << lightPos.y << ' ' << lightPos.z << std::endl;
	// std::cout << camera->position.x << ' ' << camera->position.y << ' ' << camera->position.z << std::endl;

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