#pragma once

#include "LogicalDevice.h"

class SwapChainImage {
public:
	SwapChainImage(LogicalDevice* device, VkImage img);
	void setImage(VkImage img);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

private:
	void setupImageViewCreateInfo(VkImageViewCreateInfo& createInfo, VkFormat format, VkImageAspectFlags aspectFlags);
	void createFrameBuffer();

	LogicalDevice* device;
	VkImage image;
	VkImageView imageView;
	VkFramebuffer buffer;
};

SwapChainImage::SwapChainImage(LogicalDevice* dev, VkImage img) {
	device = dev;
	image = img;
}

void SwapChainImage::createImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo createInfo = {};
	setupImageViewCreateInfo(createInfo, format, aspectFlags);

	if (vkCreateImageView(device->getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("failed to create image view.");
}

void SwapChainImage::setupImageViewCreateInfo(VkImageViewCreateInfo& createInfo, VkFormat format, VkImageAspectFlags aspectFlags) {
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
}

void SwapChainImage::createFrameBuffer() {

}