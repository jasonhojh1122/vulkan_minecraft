/*
* Vulkan Model loader using ASSIMP
*
* Copyright(C) 2016-2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Buffer.h"
#include "ModelMatrix.h"

typedef enum Component {
	VERTEX_COMPONENT_POSITION = 0x0,
	VERTEX_COMPONENT_NORMAL = 0x1,
	VERTEX_COMPONENT_COLOR = 0x2,
	VERTEX_COMPONENT_UV = 0x3,
	VERTEX_COMPONENT_TANGENT = 0x4,
	VERTEX_COMPONENT_BITANGENT = 0x5,
	VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
	VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
} Component;

struct VertexLayout {
public:
	std::vector<Component> components;

	VertexLayout(std::vector<Component> components) {
		this->components = std::move(components);
	}

	uint32_t stride() {
		uint32_t res = 0;
		for (auto& component : components) {
			switch (component)
			{
			case VERTEX_COMPONENT_UV:
				res += 2 * sizeof(float);
				break;
			case VERTEX_COMPONENT_DUMMY_FLOAT:
				res += sizeof(float);
				break;
			case VERTEX_COMPONENT_DUMMY_VEC4:
				res += 4 * sizeof(float);
				break;
			default:
				res += 3 * sizeof(float);
			}
		}
		return res;
	}

	VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = stride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> getVertexInputAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
		// position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		
		// normal
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = sizeof(float) * 3;

		// uv
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = sizeof(float) * 6;

		// color
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = sizeof(float) * 8;

		return attributeDescriptions;
	}
};

struct ModelCreateInfo {
	glm::vec3 center;
	glm::vec3 scale;
	glm::vec2 uvscale;
	VkMemoryPropertyFlags memoryPropertyFlags = 0;

	ModelCreateInfo() : center(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), uvscale(glm::vec2(1.0f)) {};

	ModelCreateInfo(glm::vec3 scale, glm::vec2 uvscale, glm::vec3 center) {
		this->center = center;
		this->scale = scale;
		this->uvscale = uvscale;
	}

	ModelCreateInfo(float scale, float uvscale, float center) {
		this->center = glm::vec3(center);
		this->scale = glm::vec3(scale);
		this->uvscale = glm::vec2(uvscale);
	}
};

class AssimpModel {
public:
	AssimpModel(LogicalDevice* inDevice, CommandPool* inPool, VertexLayout* inVertexLayout) {
		device = inDevice;
		commandPool = inPool;
		vertexLayout = inVertexLayout;
		vertexData.resize(0);
		indexData.resize(0);
		dataOffset.resize(3);
		loadModel("models/chinesedragon.dae", 0);
		loadModel("models/teapot.dae", 1);
		loadModel("models/treasure.dae", 2);
		createVertexBuffer();
		createIndexBuffer();
	}

	~AssimpModel() {
		delete vertexBuffer;
		delete indexBuffer;
	}

	Buffer* getVertexBufferRef() { return vertexBuffer; }
	Buffer* getIndexBufferRef() { return indexBuffer; }
	uint32_t getIndexOffset(int index) { return dataOffset[index].indexBase; }
	uint32_t getIndexCount(int index) { return dataOffset[index].indexCount; }
	uint32_t getVertexOffset(int index) { return dataOffset[index].vertexCount; }


private:
	LogicalDevice* device;
	CommandPool* commandPool;
	VertexLayout* vertexLayout;

	Buffer* vertexBuffer;
	Buffer* indexBuffer;

	std::vector<float> vertexData;
	std::vector<uint32_t> indexData;

	/** @brief Stores vertex and index base and counts for each part of a model */
	struct DataOffset {
		uint32_t vertexBase;
		uint32_t vertexCount;
		uint32_t indexBase;
		uint32_t indexCount;
	};
	std::vector<DataOffset> dataOffset;

	struct Dimension {
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
	};
	Dimension dim;


	static const int defaultFlags = 
		aiProcess_FlipWindingOrder | 
		aiProcess_Triangulate | 
		aiProcess_PreTransformVertices | 
		aiProcess_CalcTangentSpace | 
		aiProcess_GenSmoothNormals;

	void loadModel(const std::string& filename, int modelIndex);
	void loadFromFile(const std::string& filename, ModelCreateInfo* createInfo, int modelIndex);
	void createVertexBuffer();
	void createIndexBuffer();
};

void AssimpModel::loadModel(const std::string& filename, int modelIndex) {
	ModelCreateInfo modelCreateInfo(1.0f, 1.0f, 0.0f);
	loadFromFile(filename, &modelCreateInfo, modelIndex);
}

void AssimpModel::loadFromFile(const std::string& filename, ModelCreateInfo* createInfo, int modelIndex) {

	Assimp::Importer Importer;
	const aiScene* pScene;

	// Load file
	pScene = Importer.ReadFile(filename.c_str(), defaultFlags);
	if (!pScene) {
		std::string error = Importer.GetErrorString();
		std::cerr << error << '\n';
		throw std::runtime_error("Failed to load model from file.");
	}

	glm::vec3 scale(1.0f);
	glm::vec2 uvscale(1.0f);
	glm::vec3 center(0.0f);
	if (createInfo) {
		scale = createInfo->scale;
		uvscale = createInfo->uvscale;
		center = createInfo->center;
	}

	dataOffset[modelIndex] = {};

	// Load meshes
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];

		dataOffset[modelIndex].vertexCount += pScene->mMeshes[i]->mNumVertices;

		aiColor3D pColor(0.f, 0.f, 0.f);
		pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
			const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
			const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
			const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

			for (auto& component : vertexLayout->components) {
				switch (component) {
				case VERTEX_COMPONENT_POSITION:
					vertexData.push_back(pPos->x * scale.x + center.x);
					vertexData.push_back(-pPos->y * scale.y + center.y);
					vertexData.push_back(pPos->z * scale.z + center.z);
					break;
				case VERTEX_COMPONENT_NORMAL:
					vertexData.push_back(pNormal->x);
					vertexData.push_back(-pNormal->y);
					vertexData.push_back(pNormal->z);
					break;
				case VERTEX_COMPONENT_UV:
					vertexData.push_back(pTexCoord->x * uvscale.s);
					vertexData.push_back(pTexCoord->y * uvscale.t);
					break;
				case VERTEX_COMPONENT_COLOR:
					vertexData.push_back(pColor.r);
					vertexData.push_back(pColor.g);
					vertexData.push_back(pColor.b);
					break;
				case VERTEX_COMPONENT_TANGENT:
					vertexData.push_back(pTangent->x);
					vertexData.push_back(pTangent->y);
					vertexData.push_back(pTangent->z);
					break;
				case VERTEX_COMPONENT_BITANGENT:
					vertexData.push_back(pBiTangent->x);
					vertexData.push_back(pBiTangent->y);
					vertexData.push_back(pBiTangent->z);
					break;
				case VERTEX_COMPONENT_DUMMY_FLOAT:
					vertexData.push_back(0.0f);
					break;
				case VERTEX_COMPONENT_DUMMY_VEC4:
					vertexData.push_back(0.0f);
					vertexData.push_back(0.0f);
					vertexData.push_back(0.0f);
					vertexData.push_back(0.0f);
					break;
				};
			}

			dim.max.x = fmax(pPos->x, dim.max.x);
			dim.max.y = fmax(pPos->y, dim.max.y);
			dim.max.z = fmax(pPos->z, dim.max.z);

			dim.min.x = fmin(pPos->x, dim.min.x);
			dim.min.y = fmin(pPos->y, dim.min.y);
			dim.min.z = fmin(pPos->z, dim.min.z);
		}

		dim.size = dim.max - dim.min;

		uint32_t indexBase = static_cast<uint32_t>(indexData.size());
		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {
			const aiFace& Face = paiMesh->mFaces[j];
			if (Face.mNumIndices != 3)
				continue;
			indexData.push_back(indexBase + Face.mIndices[0]);
			indexData.push_back(indexBase + Face.mIndices[1]);
			indexData.push_back(indexBase + Face.mIndices[2]);
			dataOffset[modelIndex].indexCount += 3;
		}
	}

	if (modelIndex == 0) {
		dataOffset[modelIndex].indexBase = 0;
		dataOffset[modelIndex].vertexBase = 0;
	}
	else {
		dataOffset[modelIndex].indexBase = dataOffset[modelIndex - 1].indexBase + dataOffset[modelIndex - 1].indexCount;
		dataOffset[modelIndex].vertexBase = dataOffset[modelIndex - 1].vertexBase + dataOffset[modelIndex - 1].vertexCount;
	}
}

void AssimpModel::createIndexBuffer() {
	uint32_t iBufferSize = static_cast<uint32_t>(indexData.size()) * sizeof(uint32_t);

	Buffer* stagingBuffer = new Buffer(device,
		iBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer->copyDataToBuffer(indexData.data());

	indexBuffer = new Buffer(device,
		iBufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	indexBuffer->copyBufferToBuffer(stagingBuffer, commandPool);

	delete stagingBuffer;
	indexData.resize(0);
}

void AssimpModel::createVertexBuffer() {
	uint32_t vBufferSize = static_cast<uint32_t>(vertexData.size()) * sizeof(float);
	uint32_t iBufferSize = static_cast<uint32_t>(indexData.size()) * sizeof(uint32_t);

	Buffer* stagingBuffer = new Buffer(device,
		vBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer->copyDataToBuffer(vertexData.data());

	vertexBuffer = new Buffer(device,
		vBufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vertexBuffer->copyBufferToBuffer(stagingBuffer, commandPool);

	delete stagingBuffer;
	vertexData.resize(0);
}