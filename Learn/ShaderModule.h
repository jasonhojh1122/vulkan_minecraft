#pragma once

#include <fstream>
#include <string>
#include "LogicalDevice.h"

class ShaderModule {
public:
	~ShaderModule();
	ShaderModule(LogicalDevice& device, const std::string filename);
	VkShaderModule& getModule() { return shaderModule; }

private:
	void readFile(const std::string& filename);
	void createShaderModule();

	LogicalDevice* device;
	std::vector<char> code;
	VkShaderModule shaderModule;
};

ShaderModule::ShaderModule(LogicalDevice& dev, const std::string filename) {
	device = &dev;
	readFile(filename);
	createShaderModule();
}

void ShaderModule::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file || file.fail() == 1 || !file.is_open())
		throw std::runtime_error("Failed to open file.");
	
	size_t fileSize = static_cast<size_t>(file.tellg());
	code.resize(fileSize);
	file.seekg(0);
	file.read(code.data(), fileSize);
	file.close();
}

void ShaderModule::createShaderModule() {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = static_cast<size_t>(code.size());
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	if (vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module.");
}

ShaderModule::~ShaderModule() {
	vkDestroyShaderModule(device->getDevice(), shaderModule, nullptr);
}