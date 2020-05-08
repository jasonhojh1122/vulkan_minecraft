#pragma once

#include "LogicalDevice.h"
#include "ShaderModule.h"
#include "SwapChain.h"
#include "DescriptorSetLayout.h"
#include "RenderPass.h"
#include "AssimpModel.h"

class Pipeline {
public:
	~Pipeline();
	Pipeline(LogicalDevice* device, SwapChain* swapChain, DescriptorSetLayout* descriptorSetLayout, RenderPass* renderPass, VertexLayout* vertexLayout);
	VkPipelineLayout& getPipelineLayout() { return layout; }
	VkPipeline& getPhongPipeline() { return phong; }
	VkPipeline& getGouraudPipeline() { return gouraud; }
	VkPipeline& getFlatPipeline() { return flat; }

private:
	void createPipelineCache();
	void createGraphicsPipeline();
	void setupShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& createInfo, VkShaderStageFlagBits stage, ShaderModule& module);
	void setupVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo& createInfo,
		VkVertexInputBindingDescription& binding,
		std::vector<VkVertexInputAttributeDescription>& attributes);
	void setupInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo& createInfo);
	void setupViewport(VkViewport& viewport);
	void setupViewportStateCreateInfo(VkPipelineViewportStateCreateInfo& createInfo, VkViewport& viewport, VkRect2D& scissor);
	void setupRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo& createInfo);
	void setupDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& createInfo);
	void setupMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& createInfo);
	void setupColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& createInfo, VkPipelineColorBlendAttachmentState& attachment);
	void setupColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& attachment);

	void createPipelineLayout();
	void setupLayoutCreateInfo(VkPipelineLayoutCreateInfo& createInfo);

	LogicalDevice* device;
	SwapChain* swapChain;
	DescriptorSetLayout* descriptorSetLayout;
	RenderPass* renderPass;
	VertexLayout* vertexLayout;
	VkPipelineLayout layout;
	VkPipelineCache pipelineCache;

	VkPipeline phong;
	VkPipeline gouraud;
	VkPipeline flat;
};

Pipeline::~Pipeline() {
	vkDestroyPipeline(device->getDevice(), phong, nullptr);
	vkDestroyPipeline(device->getDevice(), gouraud, nullptr);
	vkDestroyPipeline(device->getDevice(), flat, nullptr);
	vkDestroyPipelineLayout(device->getDevice(), layout, nullptr);
	vkDestroyPipelineCache(device->getDevice(), pipelineCache, nullptr);
}

Pipeline::Pipeline(LogicalDevice* inDevice, SwapChain* inSwapChain, DescriptorSetLayout* inDescriptorSetLayout, 
	RenderPass* inRenderPass, VertexLayout* inVertexLayout) {
	device = inDevice;
	swapChain = inSwapChain;
	descriptorSetLayout = inDescriptorSetLayout;
	renderPass = inRenderPass;
	vertexLayout = inVertexLayout;

	createPipelineCache();
	createGraphicsPipeline();
}

void Pipeline::createPipelineCache() {
	VkPipelineCacheCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(device->getDevice(), &createInfo, nullptr, &pipelineCache);
}

void Pipeline::createGraphicsPipeline() {
	ShaderModule gouraudVertShader(device, "shaders/gouraud.vert.spv");
	ShaderModule gouraudFragShader(device, "shaders/gouraud.frag.spv");
	ShaderModule phongVertShader(device, "shaders/phong.vert.spv");
	ShaderModule phongFragShader(device, "shaders/phong.frag.spv");
	ShaderModule flatVertShader(device, "shaders/flat.vert.spv");
	ShaderModule flatFragShader(device, "shaders/flat.frag.spv");

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	setupInputAssemblyStateCreateInfo(inputAssembly);

	VkPipelineViewportStateCreateInfo viewportState{};
	VkViewport viewport{};
	VkRect2D scissor{};
	setupViewportStateCreateInfo(viewportState, viewport, scissor);
	
	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicState.pDynamicStates = dynamicStateEnables.data();

	VkPipelineRasterizationStateCreateInfo rasterization{};
	setupRasterizationStateCreateInfo(rasterization);

	VkPipelineMultisampleStateCreateInfo multisample{};
	setupMultisampleStateCreateInfo(multisample);

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	setupDepthStencilStateCreateInfo(depthStencil);


	VkPipelineColorBlendStateCreateInfo colorBlend{};
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	setupColorBlendStateCreateInfo(colorBlend, colorBlendAttachment);

	createPipelineLayout();

	VkPipelineVertexInputStateCreateInfo vertexInput{};
	auto bindingDescription = vertexLayout->getBindingDescription();
	auto attributeDescriptions = vertexLayout->getVertexInputAttributeDescriptions();
	setupVertexInputStateCreateInfo(vertexInput, bindingDescription, attributeDescriptions);

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterization;
	pipelineInfo.pMultisampleState = &multisample;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlend;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = renderPass->getRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;

	VkPipelineShaderStageCreateInfo shaderStages[2] = {};
	pipelineInfo.pStages = shaderStages;

	setupShaderStageCreateInfo(shaderStages[0], VK_SHADER_STAGE_VERTEX_BIT, phongVertShader);
	setupShaderStageCreateInfo(shaderStages[1], VK_SHADER_STAGE_FRAGMENT_BIT, phongFragShader);

	if (vkCreateGraphicsPipelines(device->getDevice(), pipelineCache, 1, &pipelineInfo, nullptr, &phong) != VK_SUCCESS)
		throw std::runtime_error("Failed to create phong graphic pipeline");

	pipelineInfo.basePipelineHandle = phong;
	pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	pipelineInfo.basePipelineIndex = -1;

	setupShaderStageCreateInfo(shaderStages[0], VK_SHADER_STAGE_VERTEX_BIT, gouraudVertShader);
	setupShaderStageCreateInfo(shaderStages[1], VK_SHADER_STAGE_FRAGMENT_BIT, gouraudFragShader);
	if (vkCreateGraphicsPipelines(device->getDevice(), pipelineCache, 1, &pipelineInfo, nullptr, &gouraud) != VK_SUCCESS)
		throw std::runtime_error("Failed to create gouraud graphic pipeline");

	setupShaderStageCreateInfo(shaderStages[0], VK_SHADER_STAGE_VERTEX_BIT, flatVertShader);
	setupShaderStageCreateInfo(shaderStages[1], VK_SHADER_STAGE_FRAGMENT_BIT, flatFragShader);
	if (vkCreateGraphicsPipelines(device->getDevice(), pipelineCache, 1, &pipelineInfo, nullptr, &flat) != VK_SUCCESS)
		throw std::runtime_error("Failed to create flat graphic pipeline");

}

void Pipeline::setupShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& createInfo, VkShaderStageFlagBits stage, ShaderModule& module) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = stage;
	createInfo.module = module.getModule();
	createInfo.pName = "main";
}

void Pipeline::setupVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo& createInfo,
	VkVertexInputBindingDescription& binding,
	std::vector<VkVertexInputAttributeDescription>& attributes) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo.vertexBindingDescriptionCount = 1;
	createInfo.pVertexBindingDescriptions = &binding;
	createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	createInfo.pVertexAttributeDescriptions = attributes.data();
}

void Pipeline::setupInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createInfo.primitiveRestartEnable = VK_FALSE;
}

void Pipeline::setupViewport(VkViewport& viewport) {
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = swapChain->getExtent().width;
	viewport.height = swapChain->getExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
}

void Pipeline::setupViewportStateCreateInfo(VkPipelineViewportStateCreateInfo& createInfo, VkViewport& viewport, VkRect2D& scissor) {
	setupViewport(viewport);
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain->getExtent();

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	createInfo.viewportCount = 1;
	createInfo.pViewports = &viewport;
	createInfo.scissorCount = 1;
	createInfo.pScissors = &scissor;
}

void Pipeline::setupRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.rasterizerDiscardEnable = VK_FALSE;
	createInfo.polygonMode = VK_POLYGON_MODE_FILL;
	createInfo.lineWidth = 1.0f;
	createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	createInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.depthBiasConstantFactor = 0.0f;
	createInfo.depthBiasClamp = 0.0f;
	createInfo.depthBiasSlopeFactor = 0.0f;
}

void Pipeline::setupMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.pSampleMask = nullptr;
	createInfo.alphaToCoverageEnable = VK_FALSE;
	createInfo.alphaToOneEnable = VK_FALSE;
}

void Pipeline::setupDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	createInfo.depthTestEnable = VK_TRUE;
	createInfo.depthWriteEnable = VK_TRUE;
	createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	createInfo.depthBoundsTestEnable = VK_FALSE;
	createInfo.stencilTestEnable = VK_FALSE;
}

void Pipeline::setupColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& createInfo, VkPipelineColorBlendAttachmentState& attachment) {
	setupColorBlendAttachmentState(attachment);
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	createInfo.logicOpEnable = VK_FALSE;
	createInfo.logicOp = VK_LOGIC_OP_COPY;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &attachment;
	createInfo.blendConstants[0] = 0.0f;
	createInfo.blendConstants[1] = 0.0f;
	createInfo.blendConstants[2] = 0.0f;
	createInfo.blendConstants[3] = 0.0f;
}
void Pipeline::setupColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& attachment) {
	attachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
}

void Pipeline::createPipelineLayout() {
	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = &descriptorSetLayout->getLayout();
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = nullptr;;

	if (vkCreatePipelineLayout(device->getDevice(), &layoutCreateInfo, nullptr, &layout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout");
}


