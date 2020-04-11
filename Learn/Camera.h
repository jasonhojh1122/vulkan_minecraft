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

const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 0.005f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 worldUp;
	glm::vec3 right;
	glm::vec3 up;

	float yaw;
	float pitch;

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

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float inYaw, float inPitch) : 
		front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = inYaw;
		pitch = inPitch;
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

	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = false) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		pitch -= xOffset;
		yaw -= yOffset;

		if (constrainPitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
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
		tmpFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		tmpFront.y = sin(glm::radians(pitch));
		tmpFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front = glm::normalize(tmpFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};