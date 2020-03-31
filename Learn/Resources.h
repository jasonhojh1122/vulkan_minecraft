#pragma once

#include "ImageResource.h"
#include "SwapChain.h"

class ColorResource : public ImageResource {
public:
	ColorResource(LogicalDevice& device, SwapChain& swapChain);
};

ColorResource::ColorResource(LogicalDevice& inDevice, SwapChain& inSwapChain) : 
	ImageResource(inDevice, inSwapChain.getExtent().width, inSwapChain.getExtent().height, 1) {
	createImageResource(
		device->getPhysicalDevice()->getMsaaSamples(),
		inSwapChain.getFormat(),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);
}

class DepthResource : public ImageResource {
public:
	DepthResource();
};