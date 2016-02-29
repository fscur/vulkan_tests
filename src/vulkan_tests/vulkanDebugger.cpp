#include "vulkanDebugger.h"

#include <iostream>
#include <assert.h>
#include <sstream>

VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(
    VkDebugReportFlagsEXT msgFlags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject,
    size_t location,
    int32_t msgCode,
    const char *pLayerPrefix,
    const char *pMsg,
    void *pUserData)
{
    std::ostringstream message;

    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        message << "ERROR: " << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        message << "WARNING: " << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        message << "PERFORMANCE WARNING: " << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        message << "INFO: " << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        message << "DEBUG: " << std::endl;
    }

    message << "[" << pLayerPrefix << "] Code " << msgCode << ": " << std::endl << pMsg;

    std::cerr << message.str() << std::endl << std::endl;

    /*
    * false indicates that layer should not bail-out of an
    * API call that had validation failures. This may mean that the
    * app dies inside the driver due to invalid parameter(s).
    * That's what would happen without validation layers, so we'll
    * keep that behavior here.
    */
    return false;
}

vulkanDebugger::vulkanDebugger(const VkInstance& instance) : 
    _vkInstance(instance)
{
    setup();
}

vulkanDebugger::~vulkanDebugger() 
{
    _destroyDebugReportCallback(_vkInstance, _debugReportCallback, NULL);
}

void vulkanDebugger::setup()
{
    _createDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_vkInstance, "vkCreateDebugReportCallbackEXT");
    if (!_createDebugReportCallback)
    {
        std::cout << "GetInstanceProcAddr: Unable to find vkCreateDebugReportCallbackEXT function." << std::endl;
        exit(1);
    }

    _destroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_vkInstance, "vkDestroyDebugReportCallbackEXT");
    if (!_destroyDebugReportCallback)
    {
        std::cout << "GetInstanceProcAddr: Unable to find vkDestroyDebugReportCallbackEXT function." << std::endl;
        exit(1);
    }

    VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
    debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    debugReportCreateInfo.pNext = NULL;
    debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugReportCreateInfo.pfnCallback = debugFunction;
    debugReportCreateInfo.pUserData = NULL;

    auto result = _createDebugReportCallback(_vkInstance, &debugReportCreateInfo, NULL, &_debugReportCallback);
    assert(result == VK_SUCCESS);
}