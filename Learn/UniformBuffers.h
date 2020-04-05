#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Buffer.h"
#include "SwapChain.h"

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class UniformBuffers {
public:
	~UniformBuffers();
	UniformBuffers(LogicalDevice* device, SwapChain* swapChain);
	Buffer* getBufferRef(size_t index) { return buffers[index]; }

private:
	void createUniformBuffers();

	LogicalDevice* device;
	SwapChain* swapChain;
	std::vector<Buffer*> buffers;
};

UniformBuffers::~UniformBuffers() {
	for (uint32_t i = 0; i < swapChain->getImageCount(); ++i)
		delete buffers[i];
}

UniformBuffers::UniformBuffers(LogicalDevice* inDevice, SwapChain* inSwapChain) {
	device = inDevice;
	swapChain = inSwapChain;
	buffers.resize(swapChain->getImageCount());
	createUniformBuffers();
}

void UniformBuffers::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (size_t i = 0; i < buffers.size(); ++i) {
		buffers[i] = new Buffer(device, bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}
}