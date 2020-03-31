#pragma once

#include "CommandPool.h"
#include "SwapChainImage.h"
#include <array>

class CommandBuffer {
public:
	CommandBuffer(LogicalDevice& device, CommandPool& commandPool, std::vector<SwapChainImage>& swapChainImages);

private:
	void allocateCommandBuffer();
	void setupCommandBufferAllocateInfo(VkCommandBufferAllocateInfo& allocateInfo);
	void recordCommands();
	void recordCommand();
	void setupCommandBufferBeginInfo(VkCommandBufferBeginInfo& beginInfo);
	void setupRenderPassBeginInfo(VkRenderPassBeginInfo& beginInfo);

	LogicalDevice* device;
	CommandPool* commandPool;
	std::vector<SwapChainImage*> swapChainImages;
	
	std::vector<VkCommandBuffer> commandBuffers;
};

CommandBuffer::CommandBuffer(LogicalDevice& inDevice, CommandPool& inCommandPool, std::vector<SwapChainImage>& inSwapChainImages) {
	device = &inDevice;
	commandPool = &inCommandPool;
	swapChainImages.resize(inSwapChainImages.size());
	for (int i = 0; i < inSwapChainImages.size(); ++i)
		swapChainImages[i] = &inSwapChainImages[i];
	commandBuffers.resize(inSwapChainImages.size());
	allocateCommandBuffer();
}

void CommandBuffer::allocateCommandBuffer() {
	VkCommandBufferAllocateInfo allocateInfo{};
	setupCommandBufferAllocateInfo(allocateInfo);
	if (vkAllocateCommandBuffers(device->getDevice(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers.");
}

void CommandBuffer::setupCommandBufferAllocateInfo(VkCommandBufferAllocateInfo& allocateInfo) {
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool->getCommandPool();
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = swapChainImages.size();
}

void CommandBuffer::recordCommands() {
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0.0f };

	for (uint32_t i = 0; i < swapChainImages.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo{};
		setupCommandBufferBeginInfo(beginInfo);
		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer.");


	}
}

void CommandBuffer::setupCommandBufferBeginInfo(VkCommandBufferBeginInfo& beginInfo) {
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
}

void CommandBuffer::setupRenderPassBeginInfo(VkRenderPassBeginInfo& beginInfo) {
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

}

