#pragma once

#include "SwapChain.h"
#include "Resources.h"
#include "RenderPass.h"

class Framebuffers {
public:
	~Framebuffers();
	Framebuffers(LogicalDevice* device, RenderPass* renderPass, SwapChain* swapChain);
	VkFramebuffer& getFrameBuffer(size_t index) { return framebuffers[index]; }

private:
	void createFramebuffers();

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

Framebuffers::~Framebuffers() {
	for (uint32_t i = 0; i < swapChain->getImageCount(); ++i)
		vkDestroyFramebuffer(device->getDevice(), framebuffers[i], nullptr);
}

void Framebuffers::createFramebuffers() {
	framebuffers.resize(swapChain->getImageCount());

	for (uint32_t i = 0; i < swapChain->getImageCount(); ++i) {
		std::array<VkImageView, 2> attachments = {
			swapChain->getSwapChainResourcesRef(i)->getImageView(),
			renderPass->getDepthResourceRef()->getImageView()
		};
		
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass->getRenderPass();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = swapChain->getExtent().width;
		framebufferCreateInfo.height = swapChain->getExtent().height;
		framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(device->getDevice(), &framebufferCreateInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer.");
	}
}