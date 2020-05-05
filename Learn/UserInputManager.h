#pragma once

#include <glfw/glfw3.h>
#include <cstdio>
#include <vector>
#include "Camera.h"
#include "ModelMatrix.h"

#ifdef _DEBUG
#include <conio.h>
#endif

class UserInputManager {
public:
	UserInputManager(Camera* cam) : camera(cam) {
		initModelMatrices();
	};
	void cursorManager(GLFWwindow* window, double xOffset, double yOffset);
	void scrollManager(double yOffset);
	void singleKeyManager(int key, int action);
	void keyPressManager(GLFWwindow* window, double deltaTime);
	void mousceButtonManager(GLFWwindow* window, int button, int action);
	glm::vec4 getLightPos(int i) { return lightPos[i]; };
	glm::mat4 getModelMatrix(int i) { return modelMatrices[i]->getModelMatrix(); }

private:
	void initModelMatrices();

	Camera* camera;
	double lastX = 0.0, lastY = 0.0;
	bool firstMouse = true;
	bool mouseLeftButtonIsClick = false;

	std::vector<ModelMatrix*> modelMatrices;

	int currentLight = 0;
	int currentModel = 0;
	float initPos = 15.0f;
	glm::vec4 lightPos[3] = {
		glm::vec4(-initPos, 0.0, 0.0, 0.0),
		glm::vec4(0.0, initPos, 0.0, 0.0),
		glm::vec4(0.0, 0.0, initPos, 0.0)
	};

	float dLightMovement = 0.1f;
	float dRotation = 0.5f;
	float dShear = 0.05f;
	float dModelMovement = 0.1f;
	glm::vec3 dScale = glm::vec3(0.1f, 0.1f, 0.1f);
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

void UserInputManager::singleKeyManager(int key, int action) {
	if (key == GLFW_KEY_SLASH && action == GLFW_PRESS)
		currentLight = (currentLight + 1) % 3;
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		currentModel = (currentModel + 1) % 3;
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
		lightPos[currentLight].x += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos[currentLight].x -= dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos[currentLight].y -= dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos[currentLight].y += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		lightPos[currentLight].z += dLightMovement;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		lightPos[currentLight].z -= dLightMovement;

	
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		modelMatrices[currentModel]->rotation.x += dRotation;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		modelMatrices[currentModel]->rotation.y += dRotation;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		modelMatrices[currentModel]->rotation.z += dRotation;

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		modelMatrices[currentModel]->position.x += dModelMovement;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		modelMatrices[currentModel]->position.x -= dModelMovement;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		modelMatrices[currentModel]->position.y += dModelMovement;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		modelMatrices[currentModel]->position.y -= dModelMovement;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		modelMatrices[currentModel]->position.z += dModelMovement;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		modelMatrices[currentModel]->position.z -= dModelMovement;

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		modelMatrices[currentModel]->shear.y += dShear;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		modelMatrices[currentModel]->shear.y -= dShear;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		modelMatrices[currentModel]->scale += dScale;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		modelMatrices[currentModel]->scale -= dScale;

#ifdef _DEBUG
	std::cout << std::string(100, '\n');
	printf("CurrentLight: %d, Light1: %.2f %.2f %.2f, , Light2: %.2f %.2f %.2f, , Light3: %.2f %.2f %.2f\nCameraPos: %.2f %.2f %.2f",
		currentLight+1,
		lightPos[0].x,
		lightPos[0].y,
		lightPos[0].z,
		lightPos[1].x,
		lightPos[1].y,
		lightPos[1].z,
		lightPos[2].x,
		lightPos[2].y,
		lightPos[2].z,
		camera->position.x,
		camera->position.y,
		camera->position.z
	);
#endif
	/*
	std::cout << "\rCurrentLight: " << currentLight << ", ";
	for (int i = 0; i < 3; ++i)
		std::cout << lightPos[i].x << ' ' << lightPos[i].y << ' ' << lightPos[i].z << ", ";
	std::cout << std::endl;
	*/
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

void UserInputManager::initModelMatrices() {
	modelMatrices.resize(3);
	float offset = 15.0f;
	glm::vec3 pos = glm::vec3(0.0f, -offset, 0.0f);
	glm::vec3 scale;
	for (int i = 0; i < 3; ++i) {
		if (i == 1) scale = glm::vec3(0.2f, 0.2f, 0.2f);
		else scale = glm::vec3(1.0f, 1.0f, 1.0f);
		modelMatrices[i] = new ModelMatrix(pos,
			scale,
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec3(-90.0, -90.0, 0.0));
		pos.y += offset;
	}
}