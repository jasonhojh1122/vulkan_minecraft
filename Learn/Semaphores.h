#pragma once

#include "LogicalDevice.h"

class Semaphores {
public:
	Semaphores(LogicalDevice& device, int num);

private:
	void createSemaphores();

	LogicalDevice* device;
	int num;
	std::vector<VkSemaphore> semaphores;
};

Semaphores::Semaphores(LogicalDevice& inDevice, int inNum) {
	device = &inDevice;
	num = inNum;
	semaphores.resize(num);
	createSemaphores();
}

void Semaphores::createSemaphores() {
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.flags = 0;
	for (int i = 0; i < num; ++i)
		if (vkCreateSemaphore(device->getDevice(), &createInfo, nullptr, &semaphores[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphore.");
}