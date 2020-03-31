#pragma once

#include <array>
#include "LogicalDevice.h"

class Descriptor {
public:
	Descriptor(LogicalDevice& logicalDevice);
	VkDescriptorSetLayout& getLayout() { return layout; }

private:
	void createDescriptorSetLayout();
	void setupUBOLayoutBinding(VkDescriptorSetLayoutBinding& uboLayoutBinding);
	void setupSamplerLayoutBinding(VkDescriptorSetLayoutBinding& samplerLayoutBinding);
	void setupLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo& layoutInfo, std::array<VkDescriptorSetLayoutBinding, 2>& bindings);

	LogicalDevice* device;
	VkDescriptorSetLayout layout;
};

Descriptor::Descriptor(LogicalDevice& logicalDevice) {
	device = &logicalDevice;
	createDescriptorSetLayout();
}

void Descriptor::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	setupUBOLayoutBinding(uboLayoutBinding);

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	setupSamplerLayoutBinding(samplerLayoutBinding);

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	setupLayoutCreateInfo(layoutInfo, bindings);

	if (vkCreateDescriptorSetLayout(device->getDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout.");
}

void Descriptor::setupUBOLayoutBinding(VkDescriptorSetLayoutBinding& uboLayoutBinding) {
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;
}

void Descriptor::setupSamplerLayoutBinding(VkDescriptorSetLayoutBinding& samplerLayoutBinding) {
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
}

void Descriptor::setupLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo& layoutInfo, std::array<VkDescriptorSetLayoutBinding, 2>& bindings) {
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
}