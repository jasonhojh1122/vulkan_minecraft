1. validation layer
    - vkEnumerateInstanceLayerProperties(): get available layers
    - VkLayerProperties: save layers info
    - VkDebugUtilsMessengerCreateInfoEXT
    - CreateDebugUtilsMessengerEXT()

2. get extension
    - glfwGetRequiredInstanceExtensions()

3. create instance
    - VkApplicationInfo
    - VkInstanceCreateInfo
    - vkCreateInstance()

4. create surface
    - glfwCreateWindowSurface()

5. physical device / queues
    - vkEnumeratePhysicalDevices()
    - vkGetPhysicalDeviceQueueFamilyProperties()
    - VkQueueFamilyProperties
6. logical device
    - vkCreateDevice
    - vkGetDeviceQueue
7. swapchain


- VkMemory is just a sequence of N bytes in memory.

- VkImage object adds to it e.g. information about the format (so you can address by texels, not bytes).

- VkImageView object helps select part of the VkImage (like stringView, arrayView or whathaveyou) also can help to match to some incompatible interface (by converting format on the fly).

- VkFramebuffer binds a VkImageView with an attachment.

- VkRenderpass defines which attachment will be drawn into
