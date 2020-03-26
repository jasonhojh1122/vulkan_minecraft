#pragma once

#include "PhysicalDevice.h"

class SwapChain {
public:
	SwapChain(PhysicalDevice& physicalDevice);

private:

	VkPresentModeKHR selectPresentMode();
	VkSurfaceFormatKHR selectSurfaceFormat();

	PhysicalDevice* physicalDevice;
	SwapChainSupportDetails* supportDetails;
	VkSwapchainKHR swapChain;
	VkFormat imageFormat;
	VkExtent2D extent;
	
};

SwapChain::SwapChain(PhysicalDevice& phyDevice) {
	physicalDevice = &phyDevice;
	supportDetails = &physicalDevice->getSwapChainSupportDetails();

}

VkPresentModeKHR SwapChain::selectPresentMode() {
	for (const auto& available : supportDetails->presentModes)
		if (available == VK_PRESENT_MODE_MAILBOX_KHR)
			return available;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR SwapChain::selectSurfaceFormat() {
	for (const auto& available : supportDetails->formats)
		if (available.format == VK_FORMAT_B8G8R8A8_UNORM && available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return available;
	return supportDetails->formats[0];
}