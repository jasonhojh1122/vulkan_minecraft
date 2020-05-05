#pragma once

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

class Buffer {
public:
	~Buffer();
	Buffer(LogicalDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	VkBuffer& getBuffer() { return buffer; }
	VkDeviceMemory& getMemory() { return memory; }
	void copyDataToBuffer(void *data);
	void copyDataToBufferFlush(void* data);
	void copyBufferToBuffer(Buffer* srcBuffer, CommandPool* commandPool);

private:
	void createBuffer();
	void allocateMemory();
	void bindMemory();

	LogicalDevice* device;
	VkDeviceSize size;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags properties;

	VkBuffer buffer;
	VkDeviceMemory memory;
};

Buffer::~Buffer() {
	vkDestroyBuffer(device->getDevice(), buffer, nullptr);
	vkFreeMemory(device->getDevice(), memory, nullptr);
}

Buffer::Buffer(LogicalDevice* inDevice, VkDeviceSize inSize, VkBufferUsageFlags inUsage, VkMemoryPropertyFlags inProperties) {
	device = inDevice;
	size = inSize;
	usage = inUsage;
	properties = inProperties;
	createBuffer();
	allocateMemory();
	bindMemory();
}

void Buffer::createBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create buffer.");
}

void Buffer::allocateMemory() {
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device->getPhysicalDevice()->retrieveMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory.");
}

void Buffer::bindMemory() {
	vkBindBufferMemory(device->getDevice(), buffer, memory, 0);
}

void Buffer::copyDataToBuffer(void *src) {
	void* data;
	vkMapMemory(device->getDevice(), memory, 0, size, 0, &data);
	memcpy(data, src, static_cast<size_t>(size));
	vkUnmapMemory(device->getDevice(), memory);
}

void Buffer::copyDataToBufferFlush(void* src) {
	void* data;
	vkMapMemory(device->getDevice(), memory, 0, size, 0, &data);
	memcpy(data, src, static_cast<size_t>(size));
	
	VkMappedMemoryRange range{};
	range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range.memory = memory;
	range.offset = 0;
	range.size = size;
	vkFlushMappedMemoryRanges(device->getDevice(), 1, &range);
	vkUnmapMemory(device->getDevice(), memory);
}

void Buffer::copyBufferToBuffer(Buffer* srcBuffer, CommandPool* commandPool) {
	CommandBuffer commandBuffer(device, commandPool);
	commandBuffer.beginSingalTimeCommands();

	VkBufferCopy region{};
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size = size;
	vkCmdCopyBuffer(commandBuffer.getCommandBuffer(), srcBuffer->getBuffer(), buffer, 1, &region);

	commandBuffer.endSingalTimeCommands();
}