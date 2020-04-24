#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

enum CameraMovement {
	CAM_FORWARD,
	CAM_BACKWARD,
	CAM_LEFT,
	CAM_RIGHT,
	CAM_UP,
	CAM_DOWN
};

const float PITCH = 0.0f;
const float YAW = 0.0f;
const float SPEED = 0.01f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 worldUp;
	glm::vec3 right;
	glm::vec3 up;

	float pitch;
	float yaw;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;
	

	Camera(glm::vec3 inPos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 inWorldUp = glm::vec3(0.0f, 1.0f, 0.0f), 
		float inYaw = YAW, float inPitch = PITCH) : 
		front(glm::vec3(0.0f, 0.0f, 1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		
		position = inPos;
		worldUp = inWorldUp;
		yaw = inYaw;
		pitch = inPitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float inpitch, float inyaw) : 
		front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		pitch = inpitch;
		yaw = inyaw;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void processKeyboard(CameraMovement direction, float deltaTime) {
		float velocity = movementSpeed * deltaTime;
		if (direction == CAM_FORWARD)
			position += front * velocity;
		if (direction == CAM_BACKWARD)
			position -= front * velocity;
		if (direction == CAM_RIGHT)
			position += right * velocity;
		if (direction == CAM_LEFT)
			position -= right * velocity;
		if (direction == CAM_UP)
			position += worldUp * velocity;
		if (direction == CAM_DOWN)
			position -= worldUp * velocity;
	}

	void processMouseMovement(float xOffset, float yOffset, bool constrainyaw = false) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw -= xOffset;
		pitch -= yOffset;

		if (constrainyaw) {
			if (yaw > 89.0f)
				yaw = 89.0f;
			if (yaw < -89.0f)
				yaw = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yOffset) {
		if (zoom > 1.0f && zoom <= 45.0f)
			zoom -= yOffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

private:
	void updateCameraVectors() {
		glm::vec3 tmpFront;
		tmpFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		tmpFront.y = sin(glm::radians(yaw));
		tmpFront.z = cos(glm::radians(yaw)) * sin(glm::radians(pitch));
		front = glm::normalize(tmpFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};