#pragma once

#include "ValidationDebugger.h"
#include "PhysicalDevice.h"

class LogicalDevice {
public:
	LogicalDevice(PhysicalDevice* phyDevice, ValidationDebugger* debugger);
	VkDevice& getDevice() { return device; }
	VkQueue& getGraphicQueue() { return graphicQueue; }
	VkQueue& getPresentQueue() { return presentQueue; }
	PhysicalDevice* getPhysicalDevice() { return physicalDevice; }

private:
	void createDevice();
	void retrieveQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, float queuePriority);
	void setupDeviceCreateInfo(VkDeviceCreateInfo& deviceCreateInfo, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos);
	void setupQueues();

	PhysicalDevice* physicalDevice;
	ValidationDebugger* debugger;
	VkDevice device;
	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkQueue graphicQueue;
	VkQueue presentQueue;
};

LogicalDevice::LogicalDevice(PhysicalDevice* inPhysicalDevice, ValidationDebugger* inDebugger) {
	physicalDevice = inPhysicalDevice;
	debugger = inDebugger;
	createDevice();
	setupQueues();
}

void LogicalDevice::createDevice() {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
	float queuePriority = 1.0f;
	retrieveQueueCreateInfos(queueCreateInfos, queuePriority);

	VkDeviceCreateInfo deviceCreateInfo{};
	setupDeviceCreateInfo(deviceCreateInfo, queueCreateInfos);

	if (vkCreateDevice(physicalDevice->getDevice(), &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device.");
}

void LogicalDevice::retrieveQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, float queuePriority) {
	QueueFamilyIndices familyIndices = physicalDevice->getQueueFamilyIndices();
	std::set<uint32_t> uniqueFamilyIndices = { familyIndices.graphic.value(), familyIndices.present.value() };
	for (uint32_t queueFamily : uniqueFamilyIndices) {
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueCount = 1;
		createInfo.queueFamilyIndex = queueFamily;
		createInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(createInfo);
	}
}

void LogicalDevice::setupDeviceCreateInfo(VkDeviceCreateInfo& deviceCreateInfo, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos) {
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
	deviceCreateInfo.pEnabledFeatures = &physicalDevice->getFeatures();
	if (debugger->isEnable()) {
		deviceCreateInfo.enabledLayerCount = debugger->getValidationLayersSize();
		deviceCreateInfo.ppEnabledLayerNames = debugger->getValidationLayersName().data();
	}
	else {
		deviceCreateInfo.enabledLayerCount = 0;
	}
}

void LogicalDevice::setupQueues() {
	uint32_t graphicIndex = physicalDevice->getQueueFamilyIndices().graphic.value();
	uint32_t presentIndex = physicalDevice->getQueueFamilyIndices().present.value();
	vkGetDeviceQueue(device, graphicIndex, 0, &graphicQueue);
	vkGetDeviceQueue(device, presentIndex, 0, &presentQueue);
}