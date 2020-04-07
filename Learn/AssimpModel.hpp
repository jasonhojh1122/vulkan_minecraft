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

#include "vulkan/vulkan.h"

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Vertex.h"


/** @brief Used to parametrize model loading */
struct ModelCreateInfo {
	glm::vec3 center;
	glm::vec3 scale;
	glm::vec2 uvscale;
	VkMemoryPropertyFlags memoryPropertyFlags = 0;

	ModelCreateInfo() : center(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), uvscale(glm::vec2(1.0f)) {};

	ModelCreateInfo(glm::vec3 scale, glm::vec2 uvscale, glm::vec3 center)
	{
		this->center = center;
		this->scale = scale;
		this->uvscale = uvscale;
	}

	ModelCreateInfo(float scale, float uvscale, float center)
	{
		this->center = glm::vec3(center);
		this->scale = glm::vec3(scale);
		this->uvscale = glm::vec2(uvscale);
	}

};

class AssimpModel {
public:
	~AssimpModel() {
		destroy();
	}
	AssimpModel(const std::string& filename, VertexLayout layout, float scale, LogicalDevice* device, CommandPool* commandPool) {
		ModelCreateInfo modelCreateInfo(scale, 1.0f, 0.0f);
		loadFromFile(filename, layout, &modelCreateInfo, device, commandPool);
	}
	Buffer* getVertexBufferRef() { return vertexBuffer; }
	Buffer* getIndexBufferRef() { return indexBuffer; }
	uint32_t getIndicesCount() { return indexCount; }

private:
	VkDevice device = nullptr;
	Buffer *vertexBuffer;
	Buffer *indexBuffer;
	uint32_t indexCount = 0;
	uint32_t vertexCount = 0;

	/** @brief Stores vertex and index base and counts for each part of a model */
	struct ModelPart {
		uint32_t vertexBase;
		uint32_t vertexCount;
		uint32_t indexBase;
		uint32_t indexCount;
	};
	std::vector<ModelPart> parts;

	static const int defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

	struct Dimension {
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
	} dim;

	void destroy() {
		assert(device);
		delete vertexBuffer;
		if (indexBuffer->getBuffer() != VK_NULL_HANDLE)
			delete indexBuffer;
	}

	bool loadFromFile(const std::string& filename, VertexLayout layout, ModelCreateInfo *createInfo, LogicalDevice *device, CommandPool *commandPool) {
		this->device = device->getDevice();

		Assimp::Importer Importer;
		const aiScene* pScene;

		// Load file

		pScene = Importer.ReadFile(filename.c_str(), defaultFlags);
		if (!pScene) {
			std::string error = Importer.GetErrorString();
			std::cerr << error;
			throw std::runtime_error("Failed to load model.");
		}

		if (pScene) {
			parts.clear();
			parts.resize(pScene->mNumMeshes);

			glm::vec3 scale(1.0f);
			glm::vec2 uvscale(1.0f);
			glm::vec3 center(0.0f);
			if (createInfo) {
				scale = createInfo->scale;
				uvscale = createInfo->uvscale;
				center = createInfo->center;
			}

			std::vector<float> vertexData;
			std::vector<uint32_t> indexData;

			vertexCount = 0;
			indexCount = 0;

			// Load meshes
			for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
				const aiMesh* paiMesh = pScene->mMeshes[i];

				parts[i] = {};
				parts[i].vertexBase = vertexCount;
				parts[i].indexBase = indexCount;

				vertexCount += pScene->mMeshes[i]->mNumVertices;

				aiColor3D pColor(0.f, 0.f, 0.f);
				pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

				const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

				for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
				{
					const aiVector3D* pPos = &(paiMesh->mVertices[j]);
					const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
					const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
					const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
					const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

					for (auto& component : layout.components) {
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
						// Dummy components for padding
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

				parts[i].vertexCount = paiMesh->mNumVertices;

				uint32_t indexBase = static_cast<uint32_t>(indexData.size());
				for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
				{
					const aiFace& Face = paiMesh->mFaces[j];
					if (Face.mNumIndices != 3)
						continue;
					indexData.push_back(indexBase + Face.mIndices[0]);
					indexData.push_back(indexBase + Face.mIndices[1]);
					indexData.push_back(indexBase + Face.mIndices[2]);
					parts[i].indexCount += 3;
					indexCount += 3;
				}
			}

			uint32_t vBufferSize = static_cast<uint32_t>(vertexData.size()) * sizeof(float);
			uint32_t iBufferSize = static_cast<uint32_t>(indexData.size()) * sizeof(uint32_t);

			Buffer * stagingBuffer = new Buffer(device,
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

			stagingBuffer = new Buffer(device,
				iBufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			indexBuffer = new Buffer(device,
				iBufferSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			indexBuffer->copyBufferToBuffer(stagingBuffer, commandPool);
			delete stagingBuffer;

			return true;
		}
		else
		{
			printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
			return false;
		}
	};
};
