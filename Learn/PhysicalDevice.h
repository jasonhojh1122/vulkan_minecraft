#pragma once

#include <set>
#include <stdexcept>
#include <optional>
#include <algorithm>
#include "Instance.h"
#include "Window.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphic;
	std::optional<uint32_t> present;
	bool isCompleted() { return graphic.has_value() && present.has_value();	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
	bool isEmpty() { return surfaceFormats.empty() || presentModes.empty(); }
};

class PhysicalDevice {
public:
	~PhysicalDevice() {};
	PhysicalDevice(Instance* instance, Window* win);
	VkPhysicalDevice& getDevice() { return device; }
	QueueFamilyIndices& getQueueFamilyIndices() { return queueFamilyIndices; }
	uint32_t getGraphicQueueIndex() { return queueFamilyIndices.graphic.value(); }
	uint32_t getPresentQueueIndex() { return queueFamilyIndices.present.value(); }
	SwapChainSupportDetails retrieveSwapChainSupportDetails(VkPhysicalDevice candidate, Window* win);
	VkPhysicalDeviceFeatures& getFeatures() { return features; }
	VkSampleCountFlagBits getMsaaSamples() { return msaaSamples; }
	VkFormat retrieveSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	uint32_t retrieveMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	std::vector<VkPhysicalDevice> retrievePossiblePhysicalDevice();
	void selectPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice candidate);
	bool isExtensionSupported(VkPhysicalDevice candidate);
	bool isQueueFamilySupported(VkPhysicalDevice candidate);
	bool isSwapChainSupported(VkPhysicalDevice candidate);
	bool isPhysicalDeviceFeatureSupported(VkPhysicalDevice candidate);
	void retrieveQueueFamiliesIndices(VkPhysicalDevice candidate);
	VkSampleCountFlagBits retrieveMultisampleCountFlagBits();

	Instance* vkInstance;
	Window* window;

	VkPhysicalDevice device = VK_NULL_HANDLE;

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME	};
	QueueFamilyIndices queueFamilyIndices;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memProperties;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
};

PhysicalDevice::PhysicalDevice(Instance* instance, Window* win) {
	vkInstance = instance;
	window = win;
	selectPhysicalDevice();
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
}

void PhysicalDevice::selectPhysicalDevice() {
	std::vector<VkPhysicalDevice> possibleDevices = retrievePossiblePhysicalDevice();

	for (const auto& candidate : possibleDevices) {
		if (isDeviceSuitable(candidate)) {
			device = candidate;
			msaaSamples = retrieveMultisampleCountFlagBits();
			return;
		}
	}
	throw std::runtime_error("Failed to find a GPU that meets all the requirements.");
}

std::vector<VkPhysicalDevice> PhysicalDevice::retrievePossiblePhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance->instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPUs that support Vulkan.");
	std::vector<VkPhysicalDevice> possibleDevices(deviceCount);
	vkEnumeratePhysicalDevices(vkInstance->instance, &deviceCount, possibleDevices.data());

	return possibleDevices;
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice candidate) {

	bool queueFamilySupported = isQueueFamilySupported(candidate);
	bool extensionsSupported = isExtensionSupported(candidate);
	bool swapChainSupported = isSwapChainSupported(candidate);
	bool featureSupported = isPhysicalDeviceFeatureSupported(candidate);
	
	return queueFamilySupported && extensionsSupported && swapChainSupported && featureSupported;
}

bool PhysicalDevice::isQueueFamilySupported(VkPhysicalDevice candidate) {
	retrieveQueueFamiliesIndices(candidate);
	return queueFamilyIndices.isCompleted();
}

void PhysicalDevice::retrieveQueueFamiliesIndices(VkPhysicalDevice candidate) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queueFamilyCount, queueFamilies.data());

	uint32_t index = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			queueFamilyIndices.graphic = index;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(candidate, index, window->surface, &presentSupport);

		if (presentSupport) 
			queueFamilyIndices.present = index;

		if (queueFamilyIndices.isCompleted())
			break;
		else
			index++;
	}
}

bool PhysicalDevice::isExtensionSupported(VkPhysicalDevice candidate) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(candidate, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(candidate, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> extensionsSet(extensions.begin(), extensions.end());
	for (const auto& available : availableExtensions)
		extensionsSet.erase(available.extensionName);

	return extensionsSet.empty();
}

bool PhysicalDevice::isSwapChainSupported(VkPhysicalDevice candidate) {
	SwapChainSupportDetails swapChainSupportDetails{};
	swapChainSupportDetails = retrieveSwapChainSupportDetails(candidate, window);
	return !swapChainSupportDetails.isEmpty();
}

SwapChainSupportDetails PhysicalDevice::retrieveSwapChainSupportDetails(VkPhysicalDevice candidate, Window* win) {
	SwapChainSupportDetails swapChainSupportDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(candidate, window->surface, &swapChainSupportDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, window->surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapChainSupportDetails.surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, window->surface, &formatCount, swapChainSupportDetails.surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, window->surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		swapChainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, window->surface, &presentModeCount, swapChainSupportDetails.presentModes.data());
	}

	return swapChainSupportDetails;
}

bool PhysicalDevice::isPhysicalDeviceFeatureSupported(VkPhysicalDevice candidate) {
	vkGetPhysicalDeviceFeatures(candidate, &features);
	return features.samplerAnisotropy;
}

VkFormat PhysicalDevice::retrieveSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties prop;
		vkGetPhysicalDeviceFormatProperties(device, format, &prop);
		if (tiling == VK_IMAGE_TILING_LINEAR && (prop.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (prop.optimalTilingFeatures & features) == features)
			return format;
	}
}

VkSampleCountFlagBits PhysicalDevice::retrieveMultisampleCountFlagBits() {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	VkSamplerCreateFlags counts = std::min(properties.limits.framebufferColorSampleCounts,
		properties.limits.framebufferDepthSampleCounts);

	if (counts & VK_SAMPLE_COUNT_64_BIT)
		return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT)
		return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT)
		return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)
		return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)
		return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

uint32_t PhysicalDevice::retrieveMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags requriedProp) {
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & requriedProp) == requriedProp)
			return i;
	throw std::runtime_error("Failed to find suitable memory type");
}