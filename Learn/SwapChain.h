#pragma once

#include <algorithm>
#include "LogicalDevice.h"
#include "Window.h"
#include "ImageResource.h"

class SwapChain {
public:
	SwapChain(LogicalDevice* device, Window* window);
	VkFormat getFormat() { return surfaceFormat.format; }
	VkExtent2D getExtent() { return extent; }
	uint32_t getImageCount() { return imageCount; }
	std::vector<ImageResource*>& getSwapChainResources() { return swapChainResources; }

private:
	void setupCreateInfo(VkSwapchainCreateInfoKHR& createInfo, uint32_t queueFamilyIndices[]);
	void createSwapChain();

	VkPresentModeKHR selectPresentMode();
	VkSurfaceFormatKHR selectSurfaceFormat();
	VkExtent2D retrieveExtent();
	uint32_t retrieveSwapChainImageCount();
	void createSwapChainImages();
	void createSwapChainImageViews();

	LogicalDevice* device;
	Window* window;
	SwapChainSupportDetails* supportDetails;
	
	VkSwapchainKHR swapChain;
	std::vector<ImageResource*> swapChainResources;
	VkPresentModeKHR presentMode;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;
	uint32_t imageCount;
};

SwapChain::SwapChain(LogicalDevice* inDevice, Window* inWindow) {
	device = inDevice;
	window = inWindow;
	supportDetails = &device->getPhysicalDevice()->getSwapChainSupportDetails();

	createSwapChain();
	createSwapChainImages();
	createSwapChainImageViews();
}

void SwapChain::createSwapChain() {
	presentMode = selectPresentMode();
	surfaceFormat = selectSurfaceFormat();
	extent = retrieveExtent();
	imageCount = retrieveSwapChainImageCount();

	VkSwapchainCreateInfoKHR createInfo{};
	QueueFamilyIndices indices = device->getPhysicalDevice()->getQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphic.value(), indices.present.value() };
	setupCreateInfo(createInfo, queueFamilyIndices);

	if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain.");
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
	VkSurfaceCapabilitiesKHR capabilities = device->getPhysicalDevice()->getSwapChainSupportDetails().capabilities;
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
	uint32_t count = supportDetails->capabilities.minImageCount + 1;
	if (supportDetails->capabilities.maxImageCount > 0 && count > supportDetails->capabilities.maxImageCount) {
		count = supportDetails->capabilities.maxImageCount;
	}
	return count;
}

void SwapChain::setupCreateInfo(VkSwapchainCreateInfoKHR& createInfo, uint32_t queueFamilyIndices[]) {
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = window->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
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

void SwapChain::createSwapChainImages() {
	std::vector<VkImage> swapChainImages;
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	swapChainResources.resize(imageCount);
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, swapChainImages.data());

	for (uint32_t i = 0; i < imageCount; ++i) {
		swapChainResources[i] = new ImageResource(device, extent.width, extent.height, 1);
		swapChainResources[i]->setImage(swapChainImages[i]);
		swapChainResources[i]->setFormat(getFormat());
	}
}

void SwapChain::createSwapChainImageViews() {
	for (uint32_t i = 0; i < imageCount; ++i)
		swapChainResources[i]->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
}