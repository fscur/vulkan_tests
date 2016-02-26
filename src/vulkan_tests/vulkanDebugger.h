#pragma once

#include <vulkan\vulkan.h>

class vulkanDebugger
{
private:
    static PFN_vkCreateDebugReportCallbackEXT _createDebugReportCallback;
    static PFN_vkDestroyDebugReportCallbackEXT _destroyDebugReportCallback;
    static VkDebugReportCallbackEXT _debugReportCallback;
public:
    vulkanDebugger() {};
    ~vulkanDebugger() {};
    static void setupDebugger(const VkInstance& instance);
    static void releaseDebugger(const VkInstance& instance);
};

