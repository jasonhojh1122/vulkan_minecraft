#pragma once

#include <array>
#include "DescriptorSetLayout.h"
#include "UniformBuffers.h"
#include "Texture.h"

class DescriptorSets {
public:
	~DescriptorSets() {};
	DescriptorSets(LogicalDevice* logicalDevice, DescriptorSetLayout* layout, 
		DescriptorPool* descriptorPool, UniformBuffers* uniformBuffers, Texture* texture);
	VkDescriptorSetLayout& getLayout() { return layout->getLayout(); }
	VkDescriptorSet& getDescriptorSet(size_t index) { return descriptorSets[index]; }

private:
	void createDescriptorSets();

	LogicalDevice* device;
	DescriptorSetLayout* layout;
	DescriptorPool* pool;
	UniformBuffers* uniformBuffer;
	Texture* texture;

	std::vector<VkDescriptorSet> descriptorSets;
};

DescriptorSets::DescriptorSets(LogicalDevice* inDevice, DescriptorSetLayout* inLayout,
	DescriptorPool* inDescriptorPool, UniformBuffers* inUniformBuffers, Texture* inTexture) {
	device = inDevice;
	layout = inLayout;
	pool = inDescriptorPool;
	uniformBuffer = inUniformBuffers;
	texture = inTexture;

	createDescriptorSets();
}

void DescriptorSets::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(pool->getSwapChainRef()->getImageCount(), layout->getLayout());

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->getPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(pool->getSwapChainRef()->getImageCount());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(layouts.size());
	if (vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor sets.");

	for (size_t i = 0; i < layouts.size(); ++i) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffer->getBufferRef(i)->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->getImageView();
		imageInfo.sampler = texture->getSampler();

		std::vector<VkWriteDescriptorSet> descriptorWrites(2);
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}