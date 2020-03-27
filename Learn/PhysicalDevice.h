#pragma once

#include <set>
#include <stdexcept>
#include <optional>
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
	PhysicalDevice(Instance& instance, Window& win);
	VkPhysicalDevice& getDevice() { return device; }
	QueueFamilyIndices& getQueueFamilyIndices() { return queueFamilyIndices; }
	SwapChainSupportDetails& getSwapChainSupportDetails() { return swapChainSupportDetails; }
	VkPhysicalDeviceFeatures& getFeatures() { return features; }

private:
	std::vector<VkPhysicalDevice> retrievePossiblePhysicalDevice();
	void selectPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice candidate);
	bool isExtensionSupported(VkPhysicalDevice candidate);
	bool isQueueFamilySupported(VkPhysicalDevice candidate);
	bool isSwapChainSupported(VkPhysicalDevice candidate);
	bool isPhysicalDeviceFeatureSupported(VkPhysicalDevice candidate);
	void retrieveQueueFamiliesIndices(VkPhysicalDevice candidate);
	void retrieveSwapChainSupportDetails(VkPhysicalDevice candidate);

	Instance* vkInstance;
	Window* window;
	VkPhysicalDevice device = VK_NULL_HANDLE;
	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME	};
	QueueFamilyIndices queueFamilyIndices;
	SwapChainSupportDetails swapChainSupportDetails;
	VkPhysicalDeviceFeatures features;
};

PhysicalDevice::PhysicalDevice(Instance& instance, Window& win) {
	vkInstance = &instance;
	window = &win;
	selectPhysicalDevice();
}

void PhysicalDevice::selectPhysicalDevice() {
	std::vector<VkPhysicalDevice> possibleDevices = retrievePossiblePhysicalDevice();

	for (const auto& candidate : possibleDevices) {
		if (isDeviceSuitable(candidate)) {
			device = candidate;
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
		vkGetPhysicalDeviceSurfaceSupportKHR(candidate, index, window->getSurface(), &presentSupport);

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
	retrieveSwapChainSupportDetails(candidate);
	return !swapChainSupportDetails.isEmpty();
}

void PhysicalDevice::retrieveSwapChainSupportDetails(VkPhysicalDevice candidate) {
	swapChainSupportDetails.surfaceFormats.clear();
	swapChainSupportDetails.presentModes.clear();

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(candidate, window->getSurface(), &swapChainSupportDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, window->getSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		swapChainSupportDetails.surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, window->getSurface(), &formatCount, swapChainSupportDetails.surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, window->getSurface(), &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		swapChainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, window->getSurface(), &presentModeCount, swapChainSupportDetails.presentModes.data());
	}
}

bool PhysicalDevice::isPhysicalDeviceFeatureSupported(VkPhysicalDevice candidate) {
	vkGetPhysicalDeviceFeatures(candidate, &features);
	return features.samplerAnisotropy;
}