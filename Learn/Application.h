#pragma once

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

class Application {
public:
	Application();

private:
	void init();

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
	RenderPass* renderPass;
	Framebuffers* framebuffers;
	UniformBuffers* uniformBuffers;
	DescriptorSets* descriptorSets;
	Pipeline* pipeline;
	DrawCommands* drawCommands;
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

	texture			= new Texture(device, "textures/chalet.jpg", commandPool);
	model			= new Model(device, "models/chalet.obj", commandPool);

	colorResource	= new ColorResource(device, swapChain, commandPool);
	depthResouce	= new DepthResource(device, swapChain, commandPool);

	renderPass		= new RenderPass(device, colorResource, depthResouce);

	framebuffers	= new Framebuffers(device, renderPass, swapChain);
	uniformBuffers	= new UniformBuffers(device, swapChain);

	descriptorSets	= new DescriptorSets(device, descriptorPool, uniformBuffers, texture);

	pipeline		= new Pipeline(device, swapChain, descriptorSets, renderPass);

	drawCommands	= new DrawCommands(device, swapChain, commandPool, renderPass, framebuffers, pipeline, model, descriptorSets);
}