#pragma once

#include "CommandPool.h"
#include "CommandBuffer.h"

class ImageResource {
public:
	virtual ~ImageResource();
	ImageResource(LogicalDevice* device);
	ImageResource(LogicalDevice* device, uint32_t width, uint32_t height, uint32_t mipLevels);

	void createImageResource(VkSampleCountFlagBits samples,	VkFormat format, VkImageTiling tiling, 
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect);
	void createImageView(VkImageAspectFlags aspect);

	void setFormat(VkFormat inFormat) { format = inFormat; }
	void setImage(VkImage inImage) { image = inImage; }
	
	VkFormat getFormat() { return format; }
	VkImage& getImage() { return image; }
	VkImageView& getImageView() { return imageView; }

protected:
	void setWidth(uint32_t inWidth) { width = inWidth; }
	void setHeight(uint32_t inHeight) { height = inHeight; }
	void setMipLevels(uint32_t inMipLevels) { mipLevels = inMipLevels; }
	void transitImageLayout(CommandPool* commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);
	
	LogicalDevice* device;
	uint32_t width, height, mipLevels;
	VkFormat format = VK_FORMAT_UNDEFINED;

	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImage image = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;

private:
	void createImage(VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
	void allocateImageMemory(VkMemoryPropertyFlags properties);
	void setupImageMemoryBarrier(VkImageMemoryBarrier& barrier, VkImageLayout oldLayout, VkImageLayout newLayout);
	void setupAccessMaskAndStage(VkImageMemoryBarrier& barrier, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage,
		VkImageLayout oldLayout, VkImageLayout newLayout);

};

ImageResource::~ImageResource() {
	if (imageView != VK_NULL_HANDLE)
		vkDestroyImageView(device->getDevice(), imageView, nullptr);
	if (image != VK_NULL_HANDLE)
		vkDestroyImage(device->getDevice(), image, nullptr);
	if (memory != VK_NULL_HANDLE)
		vkFreeMemory(device->getDevice(), memory, nullptr);
}

ImageResource::ImageResource(LogicalDevice* inDevice) {
	device = inDevice;
}

ImageResource::ImageResource(LogicalDevice* inDevice, uint32_t inWidth, uint32_t inHeight, uint32_t inMipLevels) {
	device = inDevice;
	width = inWidth;
	height = inHeight;
	mipLevels = inMipLevels;
}

void ImageResource::createImageResource(VkSampleCountFlagBits samples, VkFormat inFormat, VkImageTiling tiling, 
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect) {
	format = inFormat;
	createImage(samples, tiling, usage);
	allocateImageMemory(properties);
	vkBindImageMemory(device->getDevice(), image, memory, 0);
	createImageView(aspect);
}

void ImageResource::createImage(VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage) {
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

void ImageResource::createImageView(VkImageAspectFlags aspect) {
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.format = format;
	createInfo.components = {
		VK_COMPONENT_SWIZZLE_R,
		VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B,
		VK_COMPONENT_SWIZZLE_A
	};
	createInfo.subresourceRange.aspectMask = aspect;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = mipLevels;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	if (vkCreateImageView(device->getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image view.");
}

void ImageResource::transitImageLayout(CommandPool* commandPool, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkImageMemoryBarrier barrier{};
	setupImageMemoryBarrier(barrier, oldLayout, newLayout);

	VkPipelineStageFlags srcStage, dstStage;
	setupAccessMaskAndStage(barrier, srcStage, dstStage, oldLayout, newLayout);
	
	CommandBuffer buffer(device, commandPool);
	buffer.beginSingalTimeCommands();

	vkCmdPipelineBarrier(
		buffer.getCommandBuffer(),
		srcStage, dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	buffer.endSingalTimeCommands();
}

void ImageResource::setupImageMemoryBarrier(VkImageMemoryBarrier& barrier, VkImageLayout oldLayout, VkImageLayout newLayout) {
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	else
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
}

void ImageResource::setupAccessMaskAndStage(VkImageMemoryBarrier& barrier, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage, 
	VkImageLayout oldLayout, VkImageLayout newLayout) {
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else {
		throw std::runtime_error("Unsupported layout transition.");
	}
}