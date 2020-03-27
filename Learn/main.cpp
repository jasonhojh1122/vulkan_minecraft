
#include "Window.h"
#include "ValidationDebugger.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"

int main() {
	Window window(800, 600);
	ValidationDebugger debugger(true);
	Instance instance(debugger);
	window.setInstanceRef(instance);
	window.createVulkanSurface();
	PhysicalDevice physicalDevice(instance, window);

	system("pause");
	return 0;
}


