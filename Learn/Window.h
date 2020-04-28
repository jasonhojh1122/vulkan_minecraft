#pragma once

#include <stdexcept>
#include "Instance.h"
#include "UserInputManager.h"

class Window {
public:
	~Window();
	Window(int w, int h, UserInputManager* inputManager);
	void setInstanceRef(Instance* ins) { vkInstance = ins; }
	void setWidth(int inWidth) { width = inWidth; }
	void setHeight(int inHeight) { height = inHeight; }
	void createVulkanSurface();
	void destroyWindow();
	bool isResized() { return windowResized; }
	void resetResized() { windowResized = false; }

	Instance* vkInstance;
	UserInputManager* inputManager;
	int width, height;
	GLFWwindow* glfwWindow;
	VkSurfaceKHR surface;

	bool windowResized = false;
	bool firstMouse = true;
	double lastX;
	double lastY;

private:
	void createGLFWWindow();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->windowResized = true;
	}

	static void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->inputManager->cursorManager(window, xpos, ypos);
	}

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->inputManager->scrollManager(yoffset);
	}

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->inputManager->mousceButtonManager(window, button, action);
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->inputManager->singleKeyManager(key, action);
	}

};

Window::~Window() {
	vkDestroySurfaceKHR(vkInstance->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

Window::Window(int w, int h, UserInputManager* inInputManager) : width(w), height(h), inputManager(inInputManager){
	lastX = w / 2;
	lastY = h / 2;
	createGLFWWindow();
}

void Window::createGLFWWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindow = glfwCreateWindow(width, height, "Vulkan Test", nullptr, nullptr);

	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, framebufferResizeCallback);
	glfwSetCursorPosCallback(glfwWindow, cursorCallback);
	glfwSetScrollCallback(glfwWindow, scrollCallback);
	glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetKeyCallback(glfwWindow, keyCallback);
}

void Window::createVulkanSurface() {
	if (glfwCreateWindowSurface(vkInstance->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface.");
}

void Window::destroyWindow() {
	vkDestroySurfaceKHR(vkInstance->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
}