#pragma once

#include "SwapChain.h"

class DescriptorPool {
public:
	DescriptorPool(LogicalDevice* device, SwapChain* swapChain);
	VkDescriptorPool& getPool() { return pool; }
	SwapChain* getSwapChainRef() { return swapChain; }

private:
	void createDescriptorPool();
	LogicalDevice* device;
	SwapChain* swapChain;
	VkDescriptorPool pool;
};

DescriptorPool::DescriptorPool(LogicalDevice* inDevice, SwapChain* inSwapChain) {
	device = inDevice;
	swapChain = inSwapChain;
	createDescriptorPool();
}

void DescriptorPool::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 2> poolSizes;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain->getImageCount());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain->getImageCount());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swapChain->getImageCount());

	if (vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool");
}
