#pragma once

#include "LogicalDevice.h"

class CommandPool {
public:
	~CommandPool();
	CommandPool(LogicalDevice* device);
	VkCommandPool& getCommandPool() { return commandPool; }

private:
	LogicalDevice* device;
	VkCommandPool commandPool;
};

CommandPool::~CommandPool() {
	vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);
}

CommandPool::CommandPool(LogicalDevice* inDevice) {
	device = inDevice;
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = device->getPhysicalDevice()->getQueueFamilyIndices().graphic.value();
	createInfo.flags = 0;

	if (vkCreateCommandPool(device->getDevice(), &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool.");
}