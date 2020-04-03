#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

#include "Vertex.h"
#include "Buffer.h"
#include "CommandPool.h"

class Model {
public:
	Model(LogicalDevice& device, std::string path, CommandPool& commandPool);

private:
	void loadModel(std::string path);
	void createVertexBuffer();
	void createIndexBuffer();

	LogicalDevice* device;
	CommandPool* commandPool;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer* vertexBuffer;
	Buffer* indexBuffer;
};

Model::Model(LogicalDevice& inDevice, std::string path, CommandPool& inCommandPool) {
	device = &inDevice;
	commandPool = &inCommandPool;
	loadModel(path);

}

void Model::loadModel(std::string path) {
	std::string warn, err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
		throw std::runtime_error("Failed to load model.");

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void Model::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	Buffer* stagingBuffer = new Buffer(*device, bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	stagingBuffer->copyDataToBuffer(vertices.data());

	vertexBuffer = new Buffer(*device, bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vertexBuffer->copyBufferToBuffer(*stagingBuffer, *commandPool);

	delete stagingBuffer;
}

void Model::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	Buffer* stagingBuffer = new Buffer(*device, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	stagingBuffer->copyDataToBuffer(indices.data());

	indexBuffer = new Buffer(*device, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	indexBuffer->copyBufferToBuffer(*stagingBuffer, *commandPool);

	delete stagingBuffer;
}