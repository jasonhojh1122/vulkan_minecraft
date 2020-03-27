#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Image {
public:
	Image() {}
	void createImageView();

private:
	VkImage image;
	VkImageView imageView;
};