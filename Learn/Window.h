#pragma once

#include <stdexcept>
#include "Instance.h"

class Window {
public:
	~Window();
	Window(int w, int h);
	void setInstanceRef(Instance* ins) { vkInstance = ins; }
	void setWidth(int inWidth) { width = inWidth; }
	void setHeight(int inHeight) { height = inHeight; }
	void createVulkanSurface();
	void destroyWindow();
	bool isResized() { return windowResized; }
	void resetResized() { windowResized = false; }

	Instance* vkInstance;
	int width, height;
	GLFWwindow* glfwWindow;
	VkSurfaceKHR surface;

	bool windowResized = false;

private:
	void createGLFWWindow();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->windowResized = true;
	}

};

Window::~Window() {
	vkDestroySurfaceKHR(vkInstance->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

Window::Window(int w, int h) : width(w), height(h){
	createGLFWWindow();
}

void Window::createGLFWWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindow = glfwCreateWindow(width, height, "Vulkan Test", nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, framebufferResizeCallback);
}

void Window::createVulkanSurface() {
	if (glfwCreateWindowSurface(vkInstance->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface.");
}

void Window::destroyWindow() {
	vkDestroySurfaceKHR(vkInstance->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
}