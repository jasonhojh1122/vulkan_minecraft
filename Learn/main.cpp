
#include "Window.h"
#include "ValidationDebugger.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Descriptor.h"
#include "Pipeline.h"
#include "CommandPool.h"

int main() {
	Window window(800, 600);
	ValidationDebugger debugger(true);
	Instance instance(debugger);
	window.setInstanceRef(instance);
	window.createVulkanSurface();
	PhysicalDevice physicalDevice(instance, window);
	LogicalDevice device(physicalDevice, debugger);
	SwapChain swapChain(device, window);
	RenderPass renderPass(device, swapChain);
	Descriptor descriptor(device);
	Pipeline pipeline(device, swapChain, descriptor, renderPass);
	CommandPool commandPool(device);
	system("pause");
	return 0;
}


