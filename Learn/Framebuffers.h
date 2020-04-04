#pragma once

#include "SwapChain.h"
#include "Resources.h"
#include "RenderPass.h"

class Framebuffers {
public:
	Framebuffers(LogicalDevice* device, RenderPass* renderPass, SwapChain* swapChain);
	VkFramebuffer& getFrameBuffer(size_t index) { return framebuffers[index]; }

private:
	void createFramebuffers();
	void setupFramebufferCreateInfo(VkFramebufferCreateInfo& createInfo, std::array<VkImageView, 3>& attachments);

	std::vector<VkFramebuffer> framebuffers;

	LogicalDevice* device;
	RenderPass* renderPass;
	SwapChain* swapChain;
};

Framebuffers::Framebuffers(LogicalDevice* inDevice, RenderPass* inRenderPass, SwapChain* inSwapChain) {
	device = inDevice;
	renderPass = inRenderPass;
	swapChain = inSwapChain;
	createFramebuffers();
}

void Framebuffers::createFramebuffers() {
	framebuffers.resize(swapChain->getImageCount());

	for (uint32_t i = 0; i < swapChain->getImageCount(); ++i) {
		std::array<VkImageView, 3> attachments = { 
			renderPass->getColorResourceRef()->getImageView(), 
			renderPass->getDepthResourceRef()->getImageView(), 
			swapChain->getSwapChainResources()[i]->getImageView() };
		
		VkFramebufferCreateInfo framebufferCreateInfo{};
		setupFramebufferCreateInfo(framebufferCreateInfo, attachments);

		if (vkCreateFramebuffer(device->getDevice(), &framebufferCreateInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer.");
	}
}

void Framebuffers::setupFramebufferCreateInfo(VkFramebufferCreateInfo& createInfo, std::array<VkImageView, 3>& attachments) {
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass->getRenderPass();
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.width = swapChain->getExtent().width;
	createInfo.height = swapChain->getExtent().height;
	createInfo.layers = 1;
}