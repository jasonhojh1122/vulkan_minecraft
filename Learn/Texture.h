#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_master/stb_image.h>

#include "ImageResource.h"
#include "Buffer.h"

class Texture : public ImageResource {
public:
	Texture(LogicalDevice& device, std::string path, CommandPool& commandPool);

private:
	void loadTexture(std::string path);
	void copyOriginalImageToBuffer();
	void copyBufferToVulkanImage();
	void createSampler();

	LogicalDevice* device;
	CommandPool* commandPool;
	Buffer* buffer;

	stbi_uc* pixels = 0;
	VkDeviceSize imageSize;

	VkSampler sampler;
};

Texture::Texture(LogicalDevice& inDevice, std::string path, CommandPool& inCommandPool) : ImageResource(inDevice) {
	device = &inDevice;
	commandPool = &inCommandPool;
	loadTexture(path);
	copyOriginalImageToBuffer();
	createImageResource(VK_SAMPLE_COUNT_1_BIT, 
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	transitImageLayout(*commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void Texture::loadTexture(std::string path) {
	int texWidth, texHeight, texChannels;
	pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("Failed to load texture image file.");

	imageSize = static_cast<VkDeviceSize>(texWidth * texHeight * 4);
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	width = static_cast<uint32_t>(texWidth);
	height = static_cast<uint32_t>(texHeight);
}

void Texture::copyOriginalImageToBuffer() {
	buffer = new Buffer(*device, imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	buffer->copyDataToBuffer(pixels);

	stbi_image_free(pixels);
}

void Texture::copyBufferToVulkanImage() {
	CommandBuffer commandBuffer(*device, *commandPool);
	commandBuffer.beginSingalTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer.getCommandBuffer(), buffer->getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.endSingalTimeCommands();

	delete buffer;
	buffer = nullptr;
}

void Texture::createSampler() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(mipLevels);

	if (vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create sampler.");
}