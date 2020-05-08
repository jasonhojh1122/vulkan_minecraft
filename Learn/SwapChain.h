#pragma once

#include <algorithm>
#include "LogicalDevice.h"
#include "Window.h"
#include "ImageResource.h"

class SwapChain {
public:
	~SwapChain();
	SwapChain(LogicalDevice* device, Window* window);
	VkSwapchainKHR& getSwapChain() { return swapChain; }
	VkFormat getFormat() { return format; }
	VkExtent2D getExtent() { return extent; }
	uint32_t getImageCount() { return imageCount; }
	ImageResource* getSwapChainResourcesRef(uint32_t index) { return imageResources[index]; }

private:
	void createSwapChain();

	void selectPresentMode();
	void selectSurfaceFormat();
	void retrieveExtent();
	void retrieveSwapChainImageCount();
	void createSwapChainImages();
	void createSwapChainImageViews();


	LogicalDevice* device;
	Window* window;
	SwapChainSupportDetails supportDetails;


	VkSwapchainKHR swapChain;
	std::vector<VkImage> images;
	std::vector<ImageResource*> imageResources;

	VkFormat format;
	VkPresentModeKHR presentMode;
	VkColorSpaceKHR colorSpace;
	VkExtent2D extent;
	uint32_t imageCount;

	bool framebufferResized = false;
};

SwapChain::~SwapChain() {
	for (uint32_t i = 0; i < imageCount; ++i)
		vkDestroyImageView(device->getDevice(), imageResources[i]->getImageView(), nullptr);
	vkDestroySwapchainKHR(device->getDevice(), swapChain, nullptr);
}

SwapChain::SwapChain(LogicalDevice* inDevice, Window* inWindow) {
	device = inDevice;
	window = inWindow;
	supportDetails = device->getPhysicalDevice()->retrieveSwapChainSupportDetails(device->getPhysicalDevice()->getDevice(), window);

	createSwapChain();
	createSwapChainImages();
	createSwapChainImageViews();
}

void SwapChain::createSwapChain() {
	selectPresentMode();
	selectSurfaceFormat();
	retrieveExtent();
	retrieveSwapChainImageCount();

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = window->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format;
	createInfo.imageColorSpace = colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = device->getPhysicalDevice()->getQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphic.value(), indices.present.value() };
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
	createInfo.preTransform = supportDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain.");
}

void SwapChain::selectPresentMode() {
	for (const auto& available : supportDetails.presentModes)
		if (available == VK_PRESENT_MODE_MAILBOX_KHR)
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::selectSurfaceFormat() {
	for (const auto& available : supportDetails.surfaceFormats){
		if (available.format == VK_FORMAT_B8G8R8A8_UNORM && available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = available.format;
			colorSpace = available.colorSpace;
		}
	}
	format = supportDetails.surfaceFormats[0].format;
	colorSpace = supportDetails.surfaceFormats[0].colorSpace;
}

void SwapChain::retrieveExtent() {
	VkSurfaceCapabilitiesKHR capabilities = supportDetails.capabilities;
	if (capabilities.currentExtent.width != UINT32_MAX) {
		extent = capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window->glfwWindow, &width, &height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		extent = actualExtent;
	}
}

void SwapChain::retrieveSwapChainImageCount() {
	imageCount = supportDetails.capabilities.minImageCount + 1;
	if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
		imageCount = supportDetails.capabilities.maxImageCount;
}

void SwapChain::createSwapChainImages() {
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	imageResources.resize(imageCount);
	vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, images.data());

	for (uint32_t i = 0; i < imageCount; ++i) {
		imageResources[i] = new ImageResource(device, extent.width, extent.height, 1);
		imageResources[i]->setImage(images[i]);
		imageResources[i]->setFormat(format);
	}
}

void SwapChain::createSwapChainImageViews() {
	for (uint32_t i = 0; i < imageCount; ++i)
		imageResources[i]->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
}