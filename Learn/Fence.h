#pragma once

#include "LogicalDevice.h"

class Fences {
public:
	Fences(LogicalDevice& device, int num);

private:
	void createFences();

	LogicalDevice* device;
	int num;
	std::vector<VkFence> fences;
};

Fences::Fences(LogicalDevice& inDevice, int inNum) {
	device = &inDevice;
	num = inNum;
	fences.resize(num);
}

void Fences::createFences() {
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (int i = 0; i < num; ++i)
		if (vkCreateFence(device->getDevice(), &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fence.");
}