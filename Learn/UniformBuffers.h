#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <cstdlib>

#include "Buffer.h"
#include "SwapChain.h"

struct UniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 cameraPos;
	alignas(16) glm::vec4 lightPos[3];
};

struct DynamicUniformObject {
	glm::mat4* model = nullptr;
};

class UniformBuffers {
public:
	~UniformBuffers();
	UniformBuffers(LogicalDevice* device, SwapChain* swapChain);
	Buffer* getBufferRef(size_t index) { return buffers[index]; }
	Buffer* getDynamicBufferRef(size_t index) { return dynamicUboBuffers[index]; }
	uint32_t getDynamicAlignment() { return dynamicAlignment; }

	UniformBufferObject ubo{};
	DynamicUniformObject dynamicUbo{};

private:
	void createUniformBuffers();
	void createDynamicUniformBuffers();

	LogicalDevice* device;
	SwapChain* swapChain;
	std::vector<Buffer*> buffers;
	std::vector<Buffer*> dynamicUboBuffers;
	size_t dynamicAlignment;

};

UniformBuffers::~UniformBuffers() {
	for (uint32_t i = 0; i < swapChain->getImageCount(); ++i) {
		delete buffers[i];
		delete dynamicUboBuffers[i];
	}
	if (dynamicUbo.model)
		_aligned_free(dynamicUbo.model);
}

UniformBuffers::UniformBuffers(LogicalDevice* inDevice, SwapChain* inSwapChain) {
	device = inDevice;
	swapChain = inSwapChain;
	buffers.resize(swapChain->getImageCount());
	dynamicUboBuffers.resize(swapChain->getImageCount());
	createUniformBuffers();
	createDynamicUniformBuffers();
}

void UniformBuffers::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (size_t i = 0; i < buffers.size(); ++i) {
		buffers[i] = new Buffer(device, bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}
}

void UniformBuffers::createDynamicUniformBuffers() {
	size_t minUboAlighment = device->getPhysicalDevice()->getProperties().limits.minUniformBufferOffsetAlignment;
	dynamicAlignment = sizeof(glm::mat4);
	if (minUboAlighment > 0)
		dynamicAlignment = (dynamicAlignment + minUboAlighment - 1) & ~(minUboAlighment - 1);

	size_t bufferSize = 3 * dynamicAlignment;
	dynamicUbo.model = (glm::mat4*)_aligned_malloc(bufferSize, dynamicAlignment);
	assert(dynamicUbo.model);
	for (size_t i = 0; i < dynamicUboBuffers.size(); ++i) {		
		dynamicUboBuffers[i] = new Buffer(device, bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}
}
