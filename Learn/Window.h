#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
	Window(int w, int h);
	int getWidth() { return width; }
	int getHeight() { return height; }
	void setWidth(int w) { width = w; }
	void setHeight(int h) { height = h; }

private:
	int width, height;
	GLFWwindow* window;
};

Window::Window(int w, int h) {
	width = w;
	height = h;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "Vulkan Test", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);

	// todo: deal with frame buffer resize
	// glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}