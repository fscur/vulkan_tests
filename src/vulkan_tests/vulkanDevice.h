#pragma once

#include <vulkan\vulkan.h>

#include <Windows.h>
#include <assert.h>
#include <vector>

class vulkanDevice
{
private:
    std::vector<char*> _enabledLayers;
    std::vector<char*> _enabledExtensions;
    HWND _windowHandle;
    HINSTANCE _windowInstance;

    VkInstance _vkInstance;
    uint32_t _graphicsQueueFamilyIndex;
    uint32_t _deviceQueuesCount;
    VkQueueFamilyProperties* _queueProperties;
    VkPhysicalDeviceProperties* _deviceProperties;
public:
    VkPhysicalDevice primaryGpu;
    VkPhysicalDeviceMemoryProperties* memoryProperties;

    VkSurfaceKHR surface;
    VkFormat surfaceFormat;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkCommandBuffer postPresentCommandBuffer;

    VkDevice vkDevice;
    VkQueue queue;
private:
    void initializePhysicalDevice();
    void initializeQueues();
    void initializeProperties();
    void initializeDeviceSurface();
    void initializeDevice();
    void initializeCommandPool();
    void initializeCommandBuffer();
    void initializeDeviceQueue();
public:
    vulkanDevice(VkInstance vkInstance, HWND windowHandle, HINSTANCE windowInstance, std::vector<char*> enabledLayers, std::vector<char*> enabledExtensions);
    ~vulkanDevice();
};