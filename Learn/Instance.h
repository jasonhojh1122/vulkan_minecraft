#pragma once

#include <vector>
#include <stdexcept>
#include "ValidationDebugger.h"

class Instance {
public:
	~Instance();
	Instance(ValidationDebugger* debugger);
	void destroyInstance() { vkDestroyInstance(instance, nullptr); };
	VkInstance instance;

private:
	void retrieveRequiredExtensions();

	void createInstance();
	void setupApplicationInfo(VkApplicationInfo& appInfo);
	void setupInstanceCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo& appInfo);

	ValidationDebugger* debugger;
	std::vector<const char*> extensions;
};

Instance::~Instance() {
	vkDestroyInstance(instance, nullptr);
}

Instance::Instance(ValidationDebugger* inDebugger) {
	debugger = inDebugger;
	createInstance();
	if (debugger->isEnable())
		debugger->createDebugMessenger(instance, nullptr);
}

void Instance::createInstance() {
	VkApplicationInfo appInfo{};
	setupApplicationInfo(appInfo);

	VkInstanceCreateInfo instanceCreateInfo{};
	setupInstanceCreateInfo(instanceCreateInfo, appInfo);
	
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vulkan instance.");
}

void Instance::setupApplicationInfo(VkApplicationInfo& appInfo) {
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Learn";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;
}

void Instance::setupInstanceCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo &appInfo) {
	retrieveRequiredExtensions();
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (debugger->isEnable()) {
		createInfo.enabledLayerCount = debugger->getValidationLayersSize();
		createInfo.ppEnabledLayerNames = debugger->getValidationLayersName().data();
		createInfo.pNext = &debugger->getDebugUtilsMessengerCreateInfoEXT();
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
}

void Instance::retrieveRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	extensions.clear();
	for (int i = 0; i < glfwExtensionCount; ++i)
		extensions.push_back(glfwExtensions[i]);

	if (debugger->isEnable())
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}