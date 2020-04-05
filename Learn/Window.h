#pragma once

#include <stdexcept>
#include "Instance.h"

class Window {
public:
	Window(int w, int h);
	void setInstanceRef(Instance* ins) { vkInstance = ins; }
	void createVulkanSurface();
	void destroyWindow();

	Instance* vkInstance;
	int width, height;
	GLFWwindow* glfwWindow;
	VkSurfaceKHR surface;

private:
	void createGLFWWindow();
};

Window::Window(int w, int h) : width(w), height(h){
	createGLFWWindow();
}

void Window::createGLFWWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindow = glfwCreateWindow(width, height, "Vulkan Test", nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindow, this);
	// TODO: deal with frame buffer resize
	// glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Window::createVulkanSurface() {
	if (glfwCreateWindowSurface(vkInstance->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface.");
}

void Window::destroyWindow() {
	vkDestroySurfaceKHR(vkInstance->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
}