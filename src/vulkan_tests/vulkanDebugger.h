#pragma once

#include <vulkan\vulkan.h>

class vulkanDebugger
{
private:
    PFN_vkCreateDebugReportCallbackEXT _createDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT _destroyDebugReportCallback;
    VkDebugReportCallbackEXT _debugReportCallback;

    const VkInstance& _vkInstance;
public:
    vulkanDebugger(const VkInstance& instance);
    ~vulkanDebugger();
    void setup();
};

