#pragma once

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "ValidationDebugger.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "DescriptorPool.h"
#include "DescriptorSets.h"
#include "Pipeline.h"
#include "CommandPool.h"
#include "Resources.h"
#include "Framebuffers.h"
#include "Texture.h"
#include "Model.h"
#include "UniformBuffers.h"
#include "DrawCommands.h"
#include "Fences.h"
#include "Semaphores.h"

const int MAX_IN_FLIGHT = 2;

class Application {
public:
	Application();
	void run();

	bool windowIsResized = false;

private:
	void init();

	void drawFrame();
	void acquireNextSwapChainImageIndex(uint32_t& imageIndex);
	void waitForSwapChainImageReady(uint32_t swapChainIndex);
	void updateUniformBuffer(uint32_t swapChainIndex);
	void setupSubmitInfo(VkSubmitInfo& submitInfo, uint32_t swapChainIndex, 
		VkSemaphore* waitSemaphores, VkSemaphore* signalSemaphores, VkPipelineStageFlags* waitStages);
	void submitDrawCommands(VkSubmitInfo& submitInfo);
	void presentImage(uint32_t* swapChainIndex, VkSemaphore* waitSemaphore);

	void windowResize();
	void cleanupSwapChainRelated();
	void recreateSwapChainRelated();

	Window* window;
	ValidationDebugger* debugger;
	Instance* instance;
	PhysicalDevice* physicalDevice;
	LogicalDevice* device;
	SwapChain* swapChain;
	CommandPool* commandPool;
	DescriptorPool* descriptorPool;
	Texture* texture;
	Model* model;
	ColorResource* colorResource;
	DepthResource* depthResouce;
	DescriptorSetLayout* descriptorSetLayout;
	RenderPass* renderPass;
	Framebuffers* framebuffers;
	UniformBuffers* uniformBuffers;
	DescriptorSets* descriptorSets;
	Pipeline* pipeline;
	DrawCommands* drawCommands;

	Semaphores* imageIsReadyForRenderSemaphores;
	Semaphores* imageFinishedRenderSemaphores;
	Fences* frameInFlightFences;
	Fences* imageInFlightFences;

	int currentFrame = 0;
};

Application::Application() {
	init();
}



void Application::init() {
	window		= new Window(800, 600);
	debugger	= new ValidationDebugger(true);
	instance	= new Instance(debugger);
	window->setInstanceRef(instance);
	window->createVulkanSurface();

	physicalDevice	= new PhysicalDevice(instance, window);
	device			= new LogicalDevice(physicalDevice, debugger);
	swapChain		= new SwapChain(device, window);

	commandPool		= new CommandPool(device);
	descriptorPool	= new DescriptorPool(device, swapChain);

	colorResource	= nullptr;
	depthResouce	= new DepthResource(device, swapChain, commandPool);

	descriptorSetLayout = new DescriptorSetLayout(device);
	renderPass		= new RenderPass(device, swapChain, colorResource, depthResouce);
	pipeline		= new Pipeline(device, swapChain, descriptorSetLayout, renderPass);

	framebuffers	= new Framebuffers(device, renderPass, swapChain);
	uniformBuffers	= new UniformBuffers(device, swapChain);

	texture			= new Texture(device, "textures/chalet.jpg", commandPool);
	model			= new Model(device, "models/chalet.obj", commandPool);

	descriptorSets	= new DescriptorSets(device, descriptorSetLayout, descriptorPool, uniformBuffers, texture);

	drawCommands	= new DrawCommands(device, swapChain, commandPool, renderPass, framebuffers, pipeline, model, descriptorSets);

	imageIsReadyForRenderSemaphores = new Semaphores(device, MAX_IN_FLIGHT);
	imageFinishedRenderSemaphores	= new Semaphores(device, MAX_IN_FLIGHT);
	frameInFlightFences				= new Fences(device, MAX_IN_FLIGHT);
	frameInFlightFences->createFences();
	imageInFlightFences				= new Fences(device, swapChain->getImageCount());
}

void Application::run() {
	while (!glfwWindowShouldClose(window->glfwWindow)) {
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(device->getDevice());
}

void Application::drawFrame() {

	vkWaitForFences(device->getDevice(), 1, &frameInFlightFences->getFence(currentFrame), VK_TRUE, UINT64_MAX);

	uint32_t swapChainIndex;
	acquireNextSwapChainImageIndex(swapChainIndex);
	waitForSwapChainImageReady(swapChainIndex);
	
	updateUniformBuffer(swapChainIndex);

	VkSubmitInfo submitInfo{};
	VkSemaphore waitSemaphores[] = { imageIsReadyForRenderSemaphores->getSemaphore(currentFrame) };
	VkSemaphore signalSemaphores[] = { imageFinishedRenderSemaphores->getSemaphore(currentFrame) };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	setupSubmitInfo(submitInfo, swapChainIndex, waitSemaphores, signalSemaphores, waitStages);

	submitDrawCommands(submitInfo);
	
	presentImage(&swapChainIndex, signalSemaphores);

	currentFrame = (currentFrame + 1) % MAX_IN_FLIGHT;
}

void Application::acquireNextSwapChainImageIndex(uint32_t& imageIndex) {
	VkResult result = vkAcquireNextImageKHR(device->getDevice(), swapChain->getSwapChain(), UINT64_MAX,
		imageIsReadyForRenderSemaphores->getSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);
	 
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		windowResize();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire next swap chain image");
	}
}

void Application::waitForSwapChainImageReady(uint32_t swapChainIndex) {
	if (imageInFlightFences->getFence(swapChainIndex) != VK_NULL_HANDLE)
		vkWaitForFences(device->getDevice(), 1, &imageInFlightFences->getFence(swapChainIndex), VK_TRUE, UINT64_MAX);
	imageInFlightFences->setFence(frameInFlightFences->getFence(currentFrame), swapChainIndex);
}

void Application::updateUniformBuffer(uint32_t swapChainIndex) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->getExtent().width / (float)swapChain->getExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	uniformBuffers->getBufferRef(swapChainIndex)->copyDataToBuffer(&ubo);
}

void Application::setupSubmitInfo(VkSubmitInfo& submitInfo, uint32_t swapChainIndex, 
	VkSemaphore *waitSemaphores, VkSemaphore* signalSemaphores, VkPipelineStageFlags* waitStages) {
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &drawCommands->getCommandBufferRef(swapChainIndex)->getCommandBuffer();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
}

void Application::submitDrawCommands(VkSubmitInfo& submitInfo) {
	frameInFlightFences->resetFence(currentFrame);
	if (vkQueueSubmit(device->getGraphicQueue(), 1, &submitInfo, frameInFlightFences->getFence(currentFrame)) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw commands");
}

void Application::presentImage(uint32_t* swapChainIndex, VkSemaphore* waitSemaphore) {
	VkPresentInfoKHR presentInfo{};
	VkSwapchainKHR swapChains[] = { swapChain->getSwapChain() };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = swapChainIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->isResized())
		windowResize();
	else if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image.");
}

void Application::windowResize() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window->glfwWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window->glfwWindow, &width, &height);
		glfwWaitEvents();
	}
	window->setWidth(width);
	window->setHeight(height);
	
	vkDeviceWaitIdle(device->getDevice());
	cleanupSwapChainRelated();
	recreateSwapChainRelated();
	window->resetResized();
}

void Application::cleanupSwapChainRelated() {
	delete depthResouce;
	delete framebuffers;
	delete drawCommands;
	delete uniformBuffers;
	delete descriptorPool;
	delete descriptorSets;
	delete pipeline;
	delete renderPass;
	delete swapChain;
}

void Application::recreateSwapChainRelated() {
	swapChain = new SwapChain(device, window);
	uniformBuffers = new UniformBuffers(device, swapChain);
	descriptorPool = new DescriptorPool(device, swapChain);
	depthResouce = new DepthResource(device, swapChain, commandPool);
	renderPass = new RenderPass(device, swapChain, colorResource, depthResouce);
	pipeline = new Pipeline(device, swapChain, descriptorSetLayout, renderPass);
	descriptorSets = new DescriptorSets(device, descriptorSetLayout, descriptorPool, uniformBuffers, texture);
	framebuffers = new Framebuffers(device, renderPass, swapChain);
	drawCommands = new DrawCommands(device, swapChain, commandPool, renderPass, framebuffers, pipeline, model, descriptorSets);
}