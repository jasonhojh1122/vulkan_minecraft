#pragma once

#include "ImageResource.h"
#include "SwapChain.h"

class ColorResource : public ImageResource {
public:
	ColorResource(LogicalDevice* device, SwapChain* swapChain, CommandPool* commandPool);

};

ColorResource::ColorResource(LogicalDevice* inDevice, SwapChain* inSwapChain, CommandPool* inCommandPool) :
	ImageResource(inDevice, inSwapChain->getExtent().width, inSwapChain->getExtent().height, 1) {
	
	createImageResource(
		device->getPhysicalDevice()->getMsaaSamples(),
		inSwapChain->getFormat(),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);
	transitImageLayout(inCommandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

class DepthResource : public ImageResource {
public:
	DepthResource(LogicalDevice* inDevice, SwapChain* inSwapChain, CommandPool* commandPool);

};

DepthResource::DepthResource(LogicalDevice* inDevice, SwapChain* inSwapChain, CommandPool* inCommandPool) :
	ImageResource(inDevice, inSwapChain->getExtent().width, inSwapChain->getExtent().height, 1){

	VkFormat depthFormat = device->getPhysicalDevice()->retrieveSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	createImageResource(
		device->getPhysicalDevice()->getMsaaSamples(),
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT);

	transitImageLayout(inCommandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

