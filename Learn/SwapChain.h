#pragma once

#include <algorithm>
#include "LogicalDevice.h"
#include "Window.h"
#include "SwapChainImage.h"

class SwapChain {
public:
	SwapChain(PhysicalDevice& physicalDevice, LogicalDevice& device, Window& window);

private:
	void setupCreateInfo(VkSwapchainCreateInfoKHR& createInfo);
	void createSwapChain();
	void createImages();

	VkPresentModeKHR selectPresentMode();
	VkSurfaceFormatKHR selectSurfaceFormat();
	VkExtent2D retrieveExtent();
	uint32_t retrieveSwapChainImageCount();

	PhysicalDevice* physicalDevice;
	LogicalDevice* device;
	Window* window;
	SwapChainSupportDetails* supportDetails;
	
	VkSwapchainKHR swapChain;
	VkPresentModeKHR presentMode;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;
	uint32_t imageCount;
	std::vector<SwapChainImage*> images;
};

SwapChain::SwapChain(PhysicalDevice& phyDevice, LogicalDevice& dev, Window& win) {
	physicalDevice = &phyDevice;
	device = &dev;
	window = &win;
	supportDetails = &physicalDevice->getSwapChainSupportDetails();

	createSwapChain();
}

VkPresentModeKHR SwapChain::selectPresentMode() {
	for (const auto& available : supportDetails->presentModes)
		if (available == VK_PRESENT_MODE_MAILBOX_KHR)
			return available;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR SwapChain::selectSurfaceFormat() {
	for (const auto& available : supportDetails->surfaceFormats)
		if (available.format == VK_FORMAT_B8G8R8A8_UNORM && available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return available;
	return supportDetails->surfaceFormats[0];
}

VkExtent2D SwapChain::retrieveExtent() {
	VkSurfaceCapabilitiesKHR capabilities = physicalDevice->getSwapChainSupportDetails().capabilities;
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window->getGLFWWIndow(), &width, &height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

uint32_t SwapChain::retrieveSwapChainImageCount() {
	uint32_t imageCount = supportDetails->capabilities.minImageCount + 1;
	if (supportDetails->capabilities.maxImageCount > 0 && imageCount > supportDetails->capabilities.maxImageCount) {
		imageCount = supportDetails->capabilities.maxImageCount;
	}
	return imageCount;
}

void SwapChain::createSwapChain() {
	presentMode = selectPresentMode();
	surfaceFormat = selectSurfaceFormat();
	extent = retrieveExtent();
	imageCount = retrieveSwapChainImageCount();

	VkSwapchainCreateInfoKHR createInfo{};
	setupCreateInfo(createInfo);

	if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain.");
}

void SwapChain::setupCreateInfo(VkSwapchainCreateInfoKHR& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = window->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphic.value(), indices.present.value() };
	if (indices.graphic.value() != indices.present.value()) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = supportDetails->capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
}

void SwapChain::createImages() {
	std::vector<VkImage> swapChainImages;
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, swapChainImages.data());

	images.resize(imageCount);
	for (int i = 0; i < imageCount; ++i) {
		images[i] = new Image();
		images[i]->setImage(swapChainImages[i]);
	}
}