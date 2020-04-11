#pragma once

#include "LogicalDevice.h"

class DescriptorSetLayout {
public:
	~DescriptorSetLayout();
	DescriptorSetLayout(LogicalDevice* device);
	VkDescriptorSetLayout& getLayout() { return layout; }

private:
	void createDescriptorSetLayout();

	LogicalDevice* device;
	VkDescriptorSetLayout layout;
};

DescriptorSetLayout::~DescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(device->getDevice(), layout, nullptr);
}

DescriptorSetLayout::DescriptorSetLayout(LogicalDevice* inDevice) {
	device = inDevice;
	createDescriptorSetLayout();
}

void DescriptorSetLayout::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	/*
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	*/

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device->getDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout.");
}