#pragma once

#include "CommandPool.h"
#include <array>

class CommandBuffer {
public:
	~CommandBuffer();
	CommandBuffer(LogicalDevice* device, CommandPool* commandPool);
	VkCommandBuffer& getCommandBuffer() { return commandBuffer; }
	void beginSingalTimeCommands();
	void endSingalTimeCommands();
	void beginCommands();
	void endCommands();

private:
	void allocateCommandBuffer();
	void setupCommandBufferAllocateInfo(VkCommandBufferAllocateInfo& allocateInfo);
	void setupCommandBufferBeginInfo(VkCommandBufferBeginInfo& beginInfo);

	LogicalDevice* device;
	CommandPool* commandPool;
	VkCommandBuffer commandBuffer;
};

CommandBuffer::~CommandBuffer() {
	vkFreeCommandBuffers(device->getDevice(), commandPool->getCommandPool(), 1, &commandBuffer);
}

CommandBuffer::CommandBuffer(LogicalDevice* inDevice, CommandPool* inCommandPool) {
	device = inDevice;
	commandPool = inCommandPool;
	allocateCommandBuffer();
}

void CommandBuffer::allocateCommandBuffer() {
	VkCommandBufferAllocateInfo allocateInfo{};
	setupCommandBufferAllocateInfo(allocateInfo);
	if (vkAllocateCommandBuffers(device->getDevice(), &allocateInfo, &commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers.");
}

void CommandBuffer::setupCommandBufferAllocateInfo(VkCommandBufferAllocateInfo& allocateInfo) {
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = commandPool->getCommandPool();
	allocateInfo.commandBufferCount = 1;
}

void CommandBuffer::setupCommandBufferBeginInfo(VkCommandBufferBeginInfo& beginInfo) {
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
}

void CommandBuffer::beginSingalTimeCommands() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin command buffer.");
}

void CommandBuffer::endSingalTimeCommands() {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to end recording command buffer.");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->getGraphicQueue());
}

void CommandBuffer::beginCommands() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin command buffer.");
}

void CommandBuffer::endCommands() {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to end recording command buffer.");
}