#pragma once

#include <array>
#include "LogicalDevice.h"
#include "UniformBuffers.h"
#include "Texture.h"

class DescriptorSets {
public:
	DescriptorSets(LogicalDevice* logicalDevice, DescriptorPool* descriptorPool, UniformBuffers* uniformBuffers, Texture* texture);
	VkDescriptorSetLayout& getLayout() { return layout; }
	VkDescriptorSet& getDescriptorSet(size_t index) { return descriptorSets[index]; }

private:
	void createDescriptorSetLayout();
	void setupUBOLayoutBinding(VkDescriptorSetLayoutBinding& uboLayoutBinding);
	void setupSamplerLayoutBinding(VkDescriptorSetLayoutBinding& samplerLayoutBinding);
	void setupLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo& layoutInfo, std::array<VkDescriptorSetLayoutBinding, 2>& bindings);

	void createDescriptorSets();
	void setupDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo& allocInfo, std::vector<VkDescriptorSetLayout>& layouts);
	void setupDescriptorBufferInfo(VkDescriptorBufferInfo& bufferInfo, size_t index);
	void setupDescriptorImageInfo(VkDescriptorImageInfo& imageInfo);
	void setupWirteDescriptorSets(std::vector<VkWriteDescriptorSet>& descriptorWrites, 
		VkDescriptorBufferInfo& bufferInfo, VkDescriptorImageInfo& imageInfo, size_t index);

	LogicalDevice* device;
	DescriptorPool* pool;
	UniformBuffers* uniformBuffer;
	Texture* texture;

	VkDescriptorSetLayout layout;
	std::vector<VkDescriptorSet> descriptorSets;
};

DescriptorSets::DescriptorSets(LogicalDevice* inDevice, DescriptorPool* inDescriptorPool, UniformBuffers* inUniformBuffers, Texture* inTexture) {
	device = inDevice;
	pool = inDescriptorPool;
	uniformBuffer = inUniformBuffers;
	texture = inTexture;
	createDescriptorSetLayout();
	createDescriptorSets();
}

void DescriptorSets::createDescriptorSetLayout() {

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

void DescriptorSets::setupUBOLayoutBinding(VkDescriptorSetLayoutBinding& uboLayoutBinding) {
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;
}

void DescriptorSets::setupSamplerLayoutBinding(VkDescriptorSetLayoutBinding& samplerLayoutBinding) {
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
}

void DescriptorSets::setupLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo& layoutInfo, std::array<VkDescriptorSetLayoutBinding, 2>& bindings) {
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
}

void DescriptorSets::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(pool->getSwapChainRef()->getImageCount(), layout);
	VkDescriptorSetAllocateInfo allocInfo{};
	setupDescriptorSetAllocateInfo(allocInfo, layouts);

	descriptorSets.resize(layouts.size());
	if (vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor sets.");

	for (size_t i = 0; i < layouts.size(); ++i) {
		VkDescriptorBufferInfo bufferInfo{};
		setupDescriptorBufferInfo(bufferInfo, i);

		VkDescriptorImageInfo imageInfo{};
		setupDescriptorImageInfo(imageInfo);

		std::vector<VkWriteDescriptorSet> descriptorWrites(2);
		setupWirteDescriptorSets(descriptorWrites, bufferInfo, imageInfo, i);

		vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void DescriptorSets::setupDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo& allocInfo, std::vector<VkDescriptorSetLayout>& layouts) {
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->getPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(pool->getSwapChainRef()->getImageCount());
	allocInfo.pSetLayouts = layouts.data();
}

void DescriptorSets::setupDescriptorBufferInfo(VkDescriptorBufferInfo& bufferInfo, size_t index) {
	bufferInfo.buffer = uniformBuffer->getBufferRef(index)->getBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);
}

void DescriptorSets::setupDescriptorImageInfo(VkDescriptorImageInfo& imageInfo) {
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture->getImageView();
	imageInfo.sampler = texture->getSampler();
}

void DescriptorSets::setupWirteDescriptorSets(std::vector<VkWriteDescriptorSet>& descriptorWrites,
	VkDescriptorBufferInfo& bufferInfo, VkDescriptorImageInfo& imageInfo, size_t index) {
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSets[index];
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSets[index];
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;
}