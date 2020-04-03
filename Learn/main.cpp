#include "Window.h"
#include "ValidationDebugger.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Descriptor.h"
#include "Pipeline.h"
#include "CommandPool.h"
#include "Resources.h"
#include "Framebuffers.h"
#include "Texture.h"
#include "Model.h"

int main() {
	Window window(800, 600);
	ValidationDebugger debugger(true);
	Instance instance(debugger);
	window.setInstanceRef(instance);
	window.createVulkanSurface();
	PhysicalDevice physicalDevice(instance, window);
	LogicalDevice device(physicalDevice, debugger);
	SwapChain swapChain(device, window);
	CommandPool commandPool(device);
	ColorResource colorResource(device, swapChain, commandPool);
	DepthResource depthResource(device, swapChain, commandPool);
	RenderPass renderPass(device, colorResource, depthResource);
	Framebuffers framebuffers(device, renderPass, swapChain, colorResource, depthResource);
	Descriptor descriptor(device);
	Pipeline pipeline(device, swapChain, descriptor, renderPass);
	Texture texture(device, "textures/chalet.jpg", commandPool);
	Model model(device, "models/chalet.obj", commandPool);

	system("pause");
	return 0;
}


/*

#include "App.h"

int main() {
	App app;
	app.run();
	system("pause");
	return 0;
}

*/