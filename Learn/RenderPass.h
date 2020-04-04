#pragma once

#include "Resources.h"
#include "LogicalDevice.h"

class RenderPass {
public:
	RenderPass(LogicalDevice* logicalDevice, ColorResource* colorResource, DepthResource* depthResource);
	void createRenderPass();
	VkRenderPass& getRenderPass() { return renderPass; }
	ColorResource* getColorResourceRef() { return colorResource; }
	DepthResource* getDepthResourceRef() { return depthResource; }
	
private:
	void setupAttachments(std::vector<VkAttachmentDescription>& descriptions, std::vector<VkAttachmentReference>& references);
	void setupColorAttachmentDescription(VkAttachmentDescription& description);
	void setupDepthAttachmentDescription(VkAttachmentDescription& description);
	void setupResolveAttachmentDescription(VkAttachmentDescription& description);
	void setupSubpassDescription(VkSubpassDescription& description, std::vector<VkAttachmentReference>& attachmentReference);
	void setupSubpassDependency(VkSubpassDependency& depenency);
	void setupRenderPassCreateInfo(VkRenderPassCreateInfo& createInfo, std::vector<VkAttachmentDescription>& descriptions, 
		VkSubpassDescription& subpass, VkSubpassDependency& dependency);

	LogicalDevice* device;
	ColorResource* colorResource;
	DepthResource* depthResource;
	VkRenderPass renderPass;

};

RenderPass::RenderPass(LogicalDevice* inDevice, ColorResource* inColorResource, DepthResource* inDepthResource) {
	device = inDevice;
	colorResource = inColorResource;
	depthResource = inDepthResource;
	createRenderPass();
}

void RenderPass::createRenderPass() {
	std::vector<VkAttachmentDescription> attachmentDescriptions(3);
	std::vector<VkAttachmentReference> attachmentReferences(3);
	setupAttachments(attachmentDescriptions, attachmentReferences);

	VkSubpassDescription subpassDescription{};
	setupSubpassDescription(subpassDescription, attachmentReferences);

	VkSubpassDependency depenency{};
	setupSubpassDependency(depenency);

	VkRenderPassCreateInfo createInfo{};
	setupRenderPassCreateInfo(createInfo, attachmentDescriptions, subpassDescription, depenency);

	if (vkCreateRenderPass(device->getDevice(), &createInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass");
}

void RenderPass::setupAttachments(std::vector<VkAttachmentDescription>& descriptions, std::vector<VkAttachmentReference>& references) {
	setupColorAttachmentDescription(descriptions[0]);
	references[0].attachment = 0;
	references[0].layout = descriptions[0].finalLayout;

	setupDepthAttachmentDescription(descriptions[1]);
	references[1].attachment = 1;
	references[1].layout = descriptions[1].finalLayout;

	setupResolveAttachmentDescription(descriptions[2]);
	references[2].attachment = 2;
	references[2].layout = descriptions[2].finalLayout;

}

void RenderPass::setupColorAttachmentDescription(VkAttachmentDescription& description) {
	description.format = colorResource->getFormat();
	description.samples = device->getPhysicalDevice()->getMsaaSamples();
	description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void RenderPass::setupDepthAttachmentDescription(VkAttachmentDescription& description) {
	description.format = depthResource->getFormat();
	description.samples = device->getPhysicalDevice()->getMsaaSamples();
	description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void RenderPass::setupResolveAttachmentDescription(VkAttachmentDescription& description) {
	description.format = colorResource->getFormat();
	description.samples = VK_SAMPLE_COUNT_1_BIT;
	description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void RenderPass::setupSubpassDescription(VkSubpassDescription& description, std::vector<VkAttachmentReference>& attachmentReference) {
	description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	description.colorAttachmentCount = 1;
	description.pColorAttachments = &attachmentReference[0];
	description.pDepthStencilAttachment = &attachmentReference[1];
	description.pResolveAttachments = &attachmentReference[2];
}

void RenderPass::setupSubpassDependency(VkSubpassDependency& depenency) {
	depenency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depenency.dstSubpass = 0;
	depenency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depenency.srcAccessMask = 0;
	depenency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depenency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
}

void RenderPass::setupRenderPassCreateInfo(VkRenderPassCreateInfo& createInfo,
	std::vector<VkAttachmentDescription>& attachments, 
	VkSubpassDescription& subpass, 
	VkSubpassDependency& dependency) {
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;
}