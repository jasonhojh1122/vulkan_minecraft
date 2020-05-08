#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class ModelMatrix {
public:
	ModelMatrix(glm::vec3 inPos = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 inScale = glm::vec3(1.0f, 1.0f, 1.0f), 
		glm::vec3 inShear = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 inWorldUp = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 inRotation = glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 getModelMatrix();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 shear;
	glm::vec3 worldUp;
	glm::vec3 rotation;
};

ModelMatrix::ModelMatrix(glm::vec3 inPos,
	glm::vec3 inScale,
	glm::vec3 inShear,
	glm::vec3 inWorldUp,
	glm::vec3 inRotation) {

	position = inPos;
	scale = inScale;
	shear = inShear;
	worldUp = inWorldUp;
	rotation = inRotation;
}

glm::mat4 ModelMatrix::getModelMatrix() {
	glm::mat4 matrix(1.0f);
	matrix = glm::translate(matrix, position);
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
	matrix = glm::scale(matrix, scale);
	matrix = glm::shearX3D(matrix, shear.y, shear.z);
	return matrix;
}