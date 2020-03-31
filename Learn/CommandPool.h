#pragma once

#include "LogicalDevice.h"

class CommandPool {
public:
	CommandPool(LogicalDevice& device);
	VkCommandPool& getCommandPool() { return commandPool; }

private:
	void createCommandPool();
	void setupCommandPoolCreateInfo(VkCommandPoolCreateInfo& createInfo);

	LogicalDevice* device;
	VkCommandPool commandPool;
};

CommandPool::CommandPool(LogicalDevice& inDevice) {
	device = &inDevice;
	createCommandPool();
}

void CommandPool::createCommandPool() {
	VkCommandPoolCreateInfo createInfo{};
	setupCommandPoolCreateInfo(createInfo);
	if (vkCreateCommandPool(device->getDevice(), &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool.");
}

void CommandPool::setupCommandPoolCreateInfo(VkCommandPoolCreateInfo& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = device->getPhysicalDevice()->getQueueFamilyIndices().graphic.value();
	createInfo.flags = 0;
}