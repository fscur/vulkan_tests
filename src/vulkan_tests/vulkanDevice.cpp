#include "vulkanDevice.h"

#include <iostream>

vulkanDevice::vulkanDevice(VkInstance vkInstance, HWND windowHandle, HINSTANCE windowInstance, std::vector<char*> enabledLayers, std::vector<char*> enabledExtensions) :
    _vkInstance(vkInstance),
    _windowHandle(windowHandle),
    _windowInstance(windowInstance),
    _enabledLayers(enabledLayers),
    _enabledExtensions(enabledExtensions)
{
    initializePhysicalDevice();
    initializeQueues();
    initializeProperties();
    initializeDeviceSurface();
    initializeDevice();
    initializeCommandPool();
    initializeCommandBuffer();
    initializeDeviceQueue();
}

vulkanDevice::~vulkanDevice()
{
}

void vulkanDevice::initializePhysicalDevice()
{
    uint32_t availableGpusCount;
    auto result = vkEnumeratePhysicalDevices(_vkInstance, &availableGpusCount, NULL);
    assert(result == VK_SUCCESS);
    assert(availableGpusCount);

    auto availableGpus = new VkPhysicalDevice[availableGpusCount];
    result = vkEnumeratePhysicalDevices(_vkInstance, &availableGpusCount, availableGpus);
    assert(result == VK_SUCCESS);

    primaryGpu = availableGpus[0];
}

void vulkanDevice::initializeQueues()
{
    vkGetPhysicalDeviceQueueFamilyProperties(primaryGpu, &_deviceQueuesCount, NULL);
    assert(_deviceQueuesCount >= 1);

    _queueProperties = new VkQueueFamilyProperties[_deviceQueuesCount];
    vkGetPhysicalDeviceQueueFamilyProperties(primaryGpu, &_deviceQueuesCount, _queueProperties);
    assert(_deviceQueuesCount >= 1);
}

void vulkanDevice::initializeProperties()
{
    _deviceProperties = new VkPhysicalDeviceProperties();
    vkGetPhysicalDeviceProperties(primaryGpu, _deviceProperties);

    memoryProperties = new VkPhysicalDeviceMemoryProperties();
    vkGetPhysicalDeviceMemoryProperties(primaryGpu, memoryProperties);
}

void vulkanDevice::initializeDeviceSurface()
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.hinstance = _windowInstance;
    surfaceCreateInfo.hwnd = _windowHandle;

    auto result = vkCreateWin32SurfaceKHR(_vkInstance, &surfaceCreateInfo, NULL, &surface);
    assert(result == VK_SUCCESS);

    _graphicsQueueFamilyIndex = UINT32_MAX;
    auto queueSupportsPresent = new VkBool32();
    for (uint32_t i = 0; i < _deviceQueuesCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(primaryGpu, i, surface, queueSupportsPresent);
        if (*queueSupportsPresent == VK_FALSE)
            continue;

        if (_queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _graphicsQueueFamilyIndex = i;
            break;
        }
    }
    delete queueSupportsPresent;

    if (_graphicsQueueFamilyIndex == UINT32_MAX)
    {
        std::cout << "Could not find a queue that supports both graphics and present" << std::endl;
        exit(-1);
    }

    uint32_t supportedFormatsCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(primaryGpu, surface, &supportedFormatsCount, NULL);
    assert(result == VK_SUCCESS);
    assert(supportedFormatsCount >= 1);

    auto surfaceFormats = new VkSurfaceFormatKHR[supportedFormatsCount];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(primaryGpu, surface, &supportedFormatsCount, surfaceFormats);
    assert(result == VK_SUCCESS);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (supportedFormatsCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        surfaceFormat = surfaceFormats[0].format;
    }
}

void vulkanDevice::initializeDevice()
{
    VkDeviceQueueCreateInfo queueInfo = {};

    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = new float[1]{ 0.0 };
    queueInfo.queueFamilyIndex = _graphicsQueueFamilyIndex;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = _enabledLayers.size();
    deviceInfo.ppEnabledLayerNames = _enabledLayers.data();
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = new char*[1]{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    deviceInfo.pEnabledFeatures = NULL;

    auto result = vkCreateDevice(primaryGpu, &deviceInfo, NULL, &vkDevice);
    assert(result == VK_SUCCESS);
}

void vulkanDevice::initializeCommandPool()
{
    VkCommandPoolCreateInfo commandPoolnfo = {};

    commandPoolnfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolnfo.pNext = NULL;
    commandPoolnfo.queueFamilyIndex = _graphicsQueueFamilyIndex;
    commandPoolnfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    auto result = vkCreateCommandPool(vkDevice, &commandPoolnfo, NULL, &commandPool);
    assert(result == VK_SUCCESS);
}

void vulkanDevice::initializeCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    auto result = vkAllocateCommandBuffers(vkDevice, &commandBufferAllocateInfo, &commandBuffer);
    assert(result == VK_SUCCESS);

    result = vkAllocateCommandBuffers(vkDevice, &commandBufferAllocateInfo, &postPresentCommandBuffer);
    assert(result == VK_SUCCESS);
}

void vulkanDevice::initializeDeviceQueue()
{
    vkGetDeviceQueue(vkDevice, _graphicsQueueFamilyIndex, 0, &queue);
}