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