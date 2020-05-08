#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";
	return VK_FALSE;
}

class ValidationDebugger {
public:
	ValidationDebugger(bool enableDebugger);
	
	void								createDebugMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator);
	bool								isEnable() { return enable; }
	uint32_t							getValidationLayersSize() { return static_cast<uint32_t>(validationLayers.size()); }
	std::vector<const char*>&			getValidationLayersName() { return validationLayers; }
	VkDebugUtilsMessengerCreateInfoEXT& getDebugUtilsMessengerCreateInfoEXT() { return messengerCreateInfo; }
	VkDebugUtilsMessengerEXT&			getDebugUtilsMessengerEXT() { return messenger; }
	void								destroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator);

private:
	void								setupDebugUtilsCreateInfo();
	bool								isValidationSupported();
	VkResult							createDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator);

	bool enable;
	std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
	VkDebugUtilsMessengerEXT messenger{};
};

ValidationDebugger::ValidationDebugger(bool enableDebugger) {
	enable = enableDebugger;
	if (enable) {
		if (!isValidationSupported())
			throw std::runtime_error("No support for wanted validation layers.");
		setupDebugUtilsCreateInfo();
	}
}

void ValidationDebugger::setupDebugUtilsCreateInfo() {
	messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messengerCreateInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messengerCreateInfo.pfnUserCallback = debugCallback;
	messengerCreateInfo.pUserData = nullptr;
	messengerCreateInfo.flags = 0;
}

void ValidationDebugger::createDebugMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
	if (createDebugUtilsMessengerEXT(instance, pAllocator) != VK_SUCCESS)
		throw std::runtime_error("Failed to create DebugUtilsMessengerEXT.");
}

VkResult ValidationDebugger::createDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, &messengerCreateInfo, pAllocator, &messenger);
	else 
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

bool ValidationDebugger::isValidationSupported() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) return false;
	}

	return true;
}

void ValidationDebugger::destroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, messenger, pAllocator);
	}
}