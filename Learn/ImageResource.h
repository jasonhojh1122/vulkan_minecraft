#pragma once

#include "LogicalDevice.h"

class ImageResource {
public:
	ImageResource(LogicalDevice& device, uint32_t width, uint32_t height, uint32_t mipLevels);
	void createImageResource(VkSampleCountFlagBits samples, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect);

protected:
	void createImage(VkSampleCountFlagBits samples, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage);
	void allocateImageMemory(VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspect);

	LogicalDevice* device;
	uint32_t width, height, mipLevels;

	VkDeviceMemory memory;
	VkImage image;
	VkImageView imageView;
};

ImageResource::ImageResource(LogicalDevice& inDevice, uint32_t inWidth, uint32_t inHeight, uint32_t inMipLevels) {
	device = &inDevice;
	width = inWidth;
	height = inHeight;
	mipLevels = inMipLevels;
}

void ImageResource::createImageResource(VkSampleCountFlagBits samples, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect) {
	createImage(samples, format, tiling, usage);
	allocateImageMemory(properties);
	vkBindImageMemory(device->getDevice(), image, memory, 0);
	createImageView(format, aspect);
		
}

void ImageResource::createImage(VkSampleCountFlagBits samples, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = samples;
	imageInfo.flags = 0;
	if (vkCreateImage(device->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image.");
}

void ImageResource::allocateImageMemory(VkMemoryPropertyFlags properties) {
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device->getPhysicalDevice()->retrieveMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate texture image memory.");
}

void ImageResource::createImageView(VkFormat format, VkImageAspectFlags aspect) {
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspect;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = mipLevels;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device->getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image view.");
}