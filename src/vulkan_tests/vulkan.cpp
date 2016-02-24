#include "vulkan.h"

#include "cube.h"
#include "glslCompiler.h"
#include "vulkanHelper.h"
#include "shaderLoader.h"

#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <assert.h>
#include <sstream>
#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debugFunc(VkDebugReportFlagsEXT msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location,
    int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserData)
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

vulkan::vulkan(HWND handle, HINSTANCE hInstance, int width, int height) :
    _windowHandle(handle),
    _windowHInstance(hInstance),
    _width(width),
    _height(height),
    _vertexShader(""),
    _fragmentShader("")
{
    _enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");
    _vertexShader = shaderLoader::load("..\\..\\resources\\shaders\\basic.vert");
    _fragmentShader = shaderLoader::load("..\\..\\resources\\shaders\\basic.frag");

    assert(_vertexShader != "");
    assert(_fragmentShader != "");

    //createInstance();
    //createDevice();
    //createSurface and swapchaing ?
    //device queue
    //createRenderPass();
    //createDescriptors();
    //createPipeline();

    createInstance();
    initEnumerateDevice();
    initSwapchainExtension();
    createDevice();
    registerDebugCallback();
    createCommandPool();
    createCommandBuffer();
    createDeviceQueue();
    initSwapChain();
    initDepthBuffer();
    initUniformBuffer();
    createDescriptorAndPipelineLayouts();
    createRenderpass();
    initShaders();
    initFramebuffers();
    initVertexBuffer();
    createDescriptorPool();
    createDescriptorSet();
    createPipelineCache();
    createPipeline();
    buildCommandBuffer();
    draw();
}

vulkan::~vulkan()
{
    vkDestroySemaphore(_device, _presentCompleteSemaphore, NULL);
    vkDestroyFence(_device, _drawFence, NULL);
    vkDestroyPipeline(_device, _pipeline, NULL);
    vkDestroyPipelineCache(_device, _pipelineCache, NULL);
    vkDestroyDescriptorPool(_device, _descriptorPool, NULL);

    vkDestroyBuffer(_device, _vertexBuffer.buf, NULL);
    vkFreeMemory(_device, _vertexBuffer.mem, NULL);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        vkDestroyFramebuffer(_device, _frameBuffers[i], NULL);
    }
    free(_frameBuffers);

    vkDestroyShaderModule(_device, _shaderStages[0].module, NULL);
    vkDestroyShaderModule(_device, _shaderStages[1].module, NULL);

    vkDestroyRenderPass(_device, _renderPass, NULL);

    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
    {
        vkDestroyDescriptorSetLayout(_device, _descriptorLayouts[i], NULL);
    }

    vkDestroyPipelineLayout(_device, _pipelineLayout, NULL);

    vkDestroyBuffer(_device, _uniformData.buf, NULL);
    vkFreeMemory(_device, _uniformData.mem, NULL);

    vkDestroyImageView(_device, _depth.view, NULL);
    vkDestroyImage(_device, _depth.image, NULL);
    vkFreeMemory(_device, _depth.mem, NULL);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        vkDestroyImageView(_device, _swapchainBuffers[i].view, NULL);
    }
    vkDestroySwapchainKHR(_device, _swapchain, NULL);

    vkFreeCommandBuffers(_device, _commandPool, 1, &_commandBuffer);
    vkDestroyCommandPool(_device, _commandPool, NULL);
    vkDestroyDevice(_device, NULL);
    _destroyDebugReportCallback(_vkInstance, _debugReportCallback, NULL);
    vkDestroyInstance(_vkInstance, NULL);
}

void vulkan::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "Vulkan Rocks";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "Vulkan Rocks";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;

    instanceInfo.enabledLayerCount = _enabledLayers.size();
    instanceInfo.ppEnabledLayerNames = _enabledLayers.data();

    instanceInfo.enabledExtensionCount = 3;
    instanceInfo.ppEnabledExtensionNames = new const char*[3]{ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, "VK_EXT_debug_report" };

    VkResult result = vkCreateInstance(&instanceInfo, NULL, &_vkInstance);
    assert(result == VK_SUCCESS);
}

void vulkan::initEnumerateDevice()
{
    uint32_t gpusCount;
    vkEnumeratePhysicalDevices(_vkInstance, &gpusCount, NULL);
    assert(gpusCount);

    auto gpus = new VkPhysicalDevice[gpusCount];
    vkEnumeratePhysicalDevices(_vkInstance, &gpusCount, gpus);

    _gpu = gpus[0];

    vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &_queueCount, NULL);
    assert(_queueCount >= 1);

    _queueProperties = new VkQueueFamilyProperties[_queueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &_queueCount, _queueProperties);
    assert(_queueCount >= 1);

    _memoryProperties = new VkPhysicalDeviceMemoryProperties();
    _deviceProperties = new VkPhysicalDeviceProperties();
    vkGetPhysicalDeviceMemoryProperties(_gpu, _memoryProperties);
    vkGetPhysicalDeviceProperties(_gpu, _deviceProperties);
}

void vulkan::initSwapchainExtension()
{
    VkResult res;

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.hinstance = _windowHInstance;
    createInfo.hwnd = _windowHandle;

    res = vkCreateWin32SurfaceKHR(_vkInstance, &createInfo, NULL, &_surface);
    assert(res == VK_SUCCESS);

    // Iterate over each queue to learn whether it supports presenting:
    auto supportsPresent = new VkBool32(_queueCount);
    for (uint32_t i = 0; i < _queueCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(_gpu, i, _surface, &supportsPresent[i]);
    }

    // Search for a graphics queue and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < _queueCount; i++)
    {
        if ((_queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (supportsPresent[i] == VK_TRUE)
            {
                graphicsQueueNodeIndex = i;
                break;
            }
        }
    }

    free(supportsPresent);

    // Generate error if could not find a queue that supports both a graphics
    // and present
    if (graphicsQueueNodeIndex == UINT32_MAX)
    {
        std::cout << "Could not find a queue that supports both graphics and present" << std::endl;
        exit(-1);
    }

    _graphicsQueueFamilyIndex = graphicsQueueNodeIndex;

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, _surface, &formatCount, NULL);
    assert(res == VK_SUCCESS);
    assert(formatCount >= 1);

    auto surfaceFormats = new VkSurfaceFormatKHR[formatCount];
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, _surface, &formatCount, surfaceFormats);
    assert(res == VK_SUCCESS);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        _format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        _format = surfaceFormats[0].format;
    }
}

void vulkan::createDevice()
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

    auto result = vkCreateDevice(_gpu, &deviceInfo, NULL, &_device);
    assert(result == VK_SUCCESS);
}

void vulkan::registerDebugCallback()
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
    debugReportCreateInfo.pfnCallback = debugFunc;
    debugReportCreateInfo.pUserData = NULL;

    auto result = _createDebugReportCallback(_vkInstance, &debugReportCreateInfo, NULL, &_debugReportCallback);
    assert(result == VK_SUCCESS);
}

void vulkan::createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolnfo = {};

    commandPoolnfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolnfo.pNext = NULL;
    commandPoolnfo.queueFamilyIndex = _graphicsQueueFamilyIndex;
    commandPoolnfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    auto result = vkCreateCommandPool(_device, &commandPoolnfo, NULL, &_commandPool);
    assert(result == VK_SUCCESS);
}

void vulkan::createCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = _commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    auto result = vkAllocateCommandBuffers(_device, &commandBufferAllocateInfo, &_commandBuffer);
    assert(result == VK_SUCCESS);
}

void vulkan::createDeviceQueue()
{
    vkGetDeviceQueue(_device, _graphicsQueueFamilyIndex, 0, &_queue);
}

void vulkan::initSwapChain()
{
    VkResult result;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_gpu, _surface, &surfaceCapabilities);
    assert(result == VK_SUCCESS);

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, _surface, &presentModeCount, NULL);
    assert(result == VK_SUCCESS);

    auto presentModes = new VkPresentModeKHR[presentModeCount];
    assert(presentModes);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, _surface, &presentModeCount, presentModes);
    assert(result == VK_SUCCESS);

    VkExtent2D swapChainExtent;
    // width and height are either both -1, or both not -1.
    if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
    {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapChainExtent.width = _width;
        swapChainExtent.height = _height;
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        swapChainExtent = surfaceCapabilities.currentExtent;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use in the swap chain (we desire to
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount + 1;
    if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapChainImages > surfaceCapabilities.maxImageCount))
    {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapChainImages = surfaceCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfaceCapabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = NULL;
    swapChainCreateInfo.surface = _surface;
    swapChainCreateInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapChainCreateInfo.imageFormat = _format;
    swapChainCreateInfo.imageExtent.width = swapChainExtent.width;
    swapChainCreateInfo.imageExtent.height = swapChainExtent.height;
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.presentMode = swapchainPresentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainCreateInfo.clipped = true;
    swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = NULL;

    result = vkCreateSwapchainKHR(_device, &swapChainCreateInfo, NULL, &_swapchain);
    assert(result == VK_SUCCESS);

    result = vkGetSwapchainImagesKHR(_device, _swapchain, &_swapchainImageCount, NULL);
    assert(result == VK_SUCCESS);

    auto swapchainImages = new VkImage[_swapchainImageCount];
    assert(swapchainImages);
    result = vkGetSwapchainImagesKHR(_device, _swapchain, &_swapchainImageCount, swapchainImages);
    assert(result == VK_SUCCESS);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        swapchainBuffer swapchaingBuffer;

        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = _format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.flags = 0;

        swapchaingBuffer.image = swapchainImages[i];
        //Produces warnings and i dont know what it does so im commenting it
        //vulkanHelper::setImageLayout(_commandBuffer, swapchaingBuffer.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        color_image_view.image = swapchaingBuffer.image;

        result = vkCreateImageView(_device, &color_image_view, NULL, &swapchaingBuffer.view);
        _swapchainBuffers.push_back(swapchaingBuffer);
        assert(result == VK_SUCCESS);
    }
    _currentBuffer = 0;

    if (NULL != presentModes)
    {
        free(presentModes);
    }
}

void vulkan::initDepthBuffer()
{
    VkResult res;
    bool pass;
    VkImageCreateInfo imageCreateInfo = {};

    _depth.format = VK_FORMAT_D16_UNORM;
    const VkFormat depthFormat = VK_FORMAT_D16_UNORM;

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(_gpu, depthFormat, &formatProperties);

    if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    }
    else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else
    {
        std::cout << "depth_format " << depthFormat << " Unsupported.\n";
        exit(-1);
    }

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = depthFormat;
    imageCreateInfo.extent.width = _width;
    imageCreateInfo.extent.height = _height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = NULL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = NULL;
    memoryAllocateInfo.allocationSize = 0;
    memoryAllocateInfo.memoryTypeIndex = 0;

    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.pNext = NULL;
    viewCreateInfo.image = VK_NULL_HANDLE;
    viewCreateInfo.format = depthFormat;
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT || depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        viewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkMemoryRequirements memReqs;

    /* Create image */
    res = vkCreateImage(_device, &imageCreateInfo, NULL, &_depth.image);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(_device, _depth.image, &memReqs);

    memoryAllocateInfo.allocationSize = memReqs.size;
    /* Use the memory properties to determine the type of memory required */
    pass = vulkanHelper::memoryTypeFromProperties(*_memoryProperties, memReqs.memoryTypeBits, 0, /* No requirements */ &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    res = vkAllocateMemory(_device, &memoryAllocateInfo, NULL, &_depth.mem);
    assert(res == VK_SUCCESS);

    /* Bind memory */
    res = vkBindImageMemory(_device, _depth.image, _depth.mem, 0);
    assert(res == VK_SUCCESS);

    //Produces warnings and i dont know what it does so im commenting it
    /* Set the image layout to depth stencil optimal */
    //vulkanHelper::setImageLayout(_commandBuffer, _depth.image, viewCreateInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    /* Create image view */
    viewCreateInfo.image = _depth.image;
    res = vkCreateImageView(_device, &viewCreateInfo, NULL, &_depth.view);
    assert(res == VK_SUCCESS);
}

void vulkan::initUniformBuffer()
{
    VkResult result;
    bool pass;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.size = sizeof(glm::mat4);
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = NULL;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;
    result = vkCreateBuffer(_device, &bufferCreateInfo, NULL, &_uniformData.buf);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(_device, _uniformData.buf, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = NULL;
    memoryAllocateInfo.memoryTypeIndex = 0;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    pass = vulkanHelper::memoryTypeFromProperties(*_memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    result = vkAllocateMemory(_device, &memoryAllocateInfo, NULL, &_uniformData.mem);
    assert(result == VK_SUCCESS);

    result = vkMapMemory(_device, _uniformData.mem, 0, memoryRequirements.size, 0, (void **)&_uniformDataBuffer);
    assert(result == VK_SUCCESS);

    result = vkBindBufferMemory(_device, _uniformData.buf, _uniformData.mem, 0);
    assert(result == VK_SUCCESS);

    _uniformData.bufferInfo.buffer = _uniformData.buf;
    _uniformData.bufferInfo.offset = 0;
    _uniformData.bufferInfo.range = sizeof(glm::mat4);
}

void vulkan::createDescriptorAndPipelineLayouts()
{
    VkDescriptorSetLayoutBinding layout_bindings[2];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    //if (use_texture) 
    //{
    //    layout_bindings[1].binding = 1;
    //    layout_bindings[1].descriptorType =
    //        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    layout_bindings[1].descriptorCount = 1;
    //    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    //    layout_bindings[1].pImmutableSamplers = NULL;
    //}

    /* Next take layout bindings and use them to create a descriptor set layout
    */
    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = NULL;
    descriptorLayout.bindingCount = /*use_texture ? 2 :*/ 1;
    descriptorLayout.pBindings = layout_bindings;

    VkResult result;

    _descriptorLayouts = new VkDescriptorSetLayout[NUM_DESCRIPTOR_SETS];
    result = vkCreateDescriptorSetLayout(_device, &descriptorLayout, NULL, _descriptorLayouts);
    assert(result == VK_SUCCESS);

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = NULL;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = _descriptorLayouts;

    result = vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, NULL, &_pipelineLayout);
    assert(result == VK_SUCCESS);
}

void vulkan::createRenderpass()
{
    /* Need attachments for render target and depth buffer */
    VkAttachmentDescription attachments[2];
    attachments[0].format = _format;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].flags = 0;

    //only for include_depth 
    attachments[1].format = _format;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = NULL;// &depthReference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = NULL;
    rpInfo.attachmentCount = 2;
    rpInfo.pAttachments = attachments;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = NULL;

    auto result = vkCreateRenderPass(_device, &rpInfo, NULL, &_renderPass);
    assert(result == VK_SUCCESS);
}

void vulkan::initShaders()
{
    VkResult result;
    bool returnValue;

    VkShaderModuleCreateInfo moduleCreateInfo;

    std::vector<unsigned int> vertexSpirv;
    _shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _shaderStages[0].pNext = NULL;
    _shaderStages[0].pSpecializationInfo = NULL;
    _shaderStages[0].flags = 0;
    _shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    _shaderStages[0].pName = "main";

    glslCompiler::compile(VK_SHADER_STAGE_VERTEX_BIT, _vertexShader.c_str(), vertexSpirv);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vertexSpirv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vertexSpirv.data();
    result = vkCreateShaderModule(_device, &moduleCreateInfo, NULL, &_shaderStages[0].module);
    assert(result == VK_SUCCESS);

    std::vector<unsigned int> fragSpirv;
    _shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _shaderStages[1].pNext = NULL;
    _shaderStages[1].pSpecializationInfo = NULL;
    _shaderStages[1].flags = 0;
    _shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    _shaderStages[1].pName = "main";

    glslCompiler::compile(VK_SHADER_STAGE_FRAGMENT_BIT, _fragmentShader.c_str(), fragSpirv);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = fragSpirv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = fragSpirv.data();
    result = vkCreateShaderModule(_device, &moduleCreateInfo, NULL, &_shaderStages[1].module);
    assert(result == VK_SUCCESS);
}

void vulkan::initFramebuffers()
{
    VkResult result;
    VkImageView attachments[2];
    attachments[1] = _depth.view;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = _renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = _width;
    frameBufferCreateInfo.height = _height;
    frameBufferCreateInfo.layers = 1;

    _frameBuffers = new VkFramebuffer[_swapchainImageCount];

    for (auto i = 0; i < _swapchainImageCount; i++)
    {
        attachments[0] = _swapchainBuffers[i].view;
        result = vkCreateFramebuffer(_device, &frameBufferCreateInfo, NULL, &_frameBuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void vulkan::initVertexBuffer()
{
    VkResult result;
    bool pass;

    auto cube = cube::create();
    //auto dataSize = sizeof(float) * 6 * 36;
    auto dataSize = sizeof(vertex) * cube->vertices.size();
    auto dataStride = sizeof(cube->vertices[0]);

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = NULL;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    result = vkCreateBuffer(_device, &bufferCreateInfo, NULL, &_vertexBuffer.buf);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(_device, _vertexBuffer.buf, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = NULL;
    memoryAllocateInfo.memoryTypeIndex = 0;

    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    pass = vulkanHelper::memoryTypeFromProperties(
        *_memoryProperties,
        memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &memoryAllocateInfo.memoryTypeIndex);

    assert(pass);

    result = vkAllocateMemory(_device, &memoryAllocateInfo, NULL, &_vertexBuffer.mem);
    assert(result == VK_SUCCESS);
    _vertexBuffer.bufferInfo.range = memoryRequirements.size;
    _vertexBuffer.bufferInfo.offset = 0;

    uint8_t *pData;
    result = vkMapMemory(_device, _vertexBuffer.mem, 0, memoryRequirements.size, 0, (void **)&pData);
    assert(result == VK_SUCCESS);

    memcpy(pData, &(cube->vertices[0]), dataSize);

    vkUnmapMemory(_device, _vertexBuffer.mem);

    result = vkBindBufferMemory(_device, _vertexBuffer.buf, _vertexBuffer.mem, 0);
    assert(result == VK_SUCCESS);

    _vertexInputBindingDescription.binding = 0;
    _vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    _vertexInputBindingDescription.stride = dataStride;

    _vertexInputAttributesDescription[0].binding = 0;
    _vertexInputAttributesDescription[0].location = 0;
    _vertexInputAttributesDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    _vertexInputAttributesDescription[0].offset = 0;
    _vertexInputAttributesDescription[1].binding = 0;
    _vertexInputAttributesDescription[1].location = 1;
    _vertexInputAttributesDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT; //use_texture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT
    _vertexInputAttributesDescription[1].offset = 12;
}

void vulkan::createDescriptorPool()
{
    VkResult result;
    VkDescriptorPoolSize typeCount[2];
    typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCount[0].descriptorCount = 1;

    //if (use_texture) 
    //{
    //    type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    type_count[1].descriptorCount = 1;
    //}

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = NULL;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = 1; //use texture ? 2 : 1
    descriptorPoolInfo.pPoolSizes = typeCount;

    result = vkCreateDescriptorPool(_device, &descriptorPoolInfo, NULL, &_descriptorPool);
    assert(result == VK_SUCCESS);
}

void vulkan::createDescriptorSet()
{
    VkResult result;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo[1];
    descriptorSetAllocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo[0].pNext = NULL;
    descriptorSetAllocateInfo[0].descriptorPool = _descriptorPool;
    descriptorSetAllocateInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorSetAllocateInfo[0].pSetLayouts = _descriptorLayouts;

    _descriptorSets = new VkDescriptorSet[NUM_DESCRIPTOR_SETS];
    result = vkAllocateDescriptorSets(_device, descriptorSetAllocateInfo, _descriptorSets);
    assert(result == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = _descriptorSets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &_uniformData.bufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    //if (use_texture)
    //{
    //    writes[1] = {};
    //    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //    writes[1].dstSet = info.desc_set[0];
    //    writes[1].dstBinding = 1;
    //    writes[1].descriptorCount = 1;
    //    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    writes[1].pImageInfo = &info.texture_data.image_info;
    //    writes[1].dstArrayElement = 0;
    //}

    vkUpdateDescriptorSets(_device, 1, writes, 0, NULL); //use_texture ? 2 : 1
}

void vulkan::createPipelineCache()
{
    VkResult result;

    VkPipelineCacheCreateInfo pipelineCache;
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = NULL;
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = NULL;
    pipelineCache.flags = 0;

    result = vkCreatePipelineCache(_device, &pipelineCache, NULL, &_pipelineCache);
    assert(result == VK_SUCCESS);
}

void vulkan::createPipeline()
{
    VkResult result;

    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = NULL;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &_vertexInputBindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = _vertexInputAttributesDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblerCreateInfo;
    inputAssemblerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblerCreateInfo.pNext = NULL;
    inputAssemblerCreateInfo.flags = 0;
    inputAssemblerCreateInfo.primitiveRestartEnable = VK_FALSE;
    inputAssemblerCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = NULL;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthClampEnable = true;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
    rasterizationStateCreateInfo.depthBiasClamp = 0;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
    rasterizationStateCreateInfo.lineWidth = 0;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.pNext = NULL;

    VkPipelineColorBlendAttachmentState colorBlendAttatchmentState = {};
    colorBlendAttatchmentState.colorWriteMask = 0xf;
    colorBlendAttatchmentState.blendEnable = VK_FALSE;
    colorBlendAttatchmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttatchmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttatchmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttatchmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttatchmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttatchmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttatchmentState;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateCreateInfo.blendConstants[0] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 1.0f;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = NULL;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    viewportStateCreateInfo.scissorCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    viewportStateCreateInfo.pScissors = NULL;
    viewportStateCreateInfo.pViewports = NULL;

    VkPipelineDepthStencilStateCreateInfo depthStateStencilCreateInfo;
    depthStateStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStateStencilCreateInfo.pNext = NULL;
    depthStateStencilCreateInfo.flags = 0;
    depthStateStencilCreateInfo.depthTestEnable = true;  //include depth
    depthStateStencilCreateInfo.depthWriteEnable = true;     //include depth
    depthStateStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStateStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStateStencilCreateInfo.stencilTestEnable = VK_FALSE;
    depthStateStencilCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    depthStateStencilCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    depthStateStencilCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStateStencilCreateInfo.back.compareMask = 0;
    depthStateStencilCreateInfo.back.reference = 0;
    depthStateStencilCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStateStencilCreateInfo.back.writeMask = 0;
    depthStateStencilCreateInfo.minDepthBounds = 0;
    depthStateStencilCreateInfo.maxDepthBounds = 0;
    depthStateStencilCreateInfo.stencilTestEnable = VK_FALSE;
    depthStateStencilCreateInfo.front = depthStateStencilCreateInfo.back;

    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo;
    multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleStateCreateInfo.pNext = NULL;
    multiSampleStateCreateInfo.flags = 0;
    multiSampleStateCreateInfo.pSampleMask = NULL;
    multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    multiSampleStateCreateInfo.minSampleShading = 0.0;

    VkGraphicsPipelineCreateInfo pipeline;
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext = NULL;
    pipeline.layout = _pipelineLayout;
    pipeline.basePipelineHandle = VK_NULL_HANDLE;
    pipeline.basePipelineIndex = 0;
    pipeline.flags = 0;
    pipeline.pVertexInputState = &vertexInputStateCreateInfo; //include_vi ? &vi : NULL;
    pipeline.pInputAssemblyState = &inputAssemblerCreateInfo;
    pipeline.pRasterizationState = &rasterizationStateCreateInfo;
    pipeline.pColorBlendState = &colorBlendStateCreateInfo;
    pipeline.pTessellationState = NULL;
    pipeline.pMultisampleState = &multiSampleStateCreateInfo;
    pipeline.pDynamicState = &dynamicState;
    pipeline.pViewportState = &viewportStateCreateInfo;
    pipeline.pDepthStencilState = &depthStateStencilCreateInfo;
    pipeline.pStages = _shaderStages;
    pipeline.stageCount = 2;
    pipeline.renderPass = _renderPass;
    pipeline.subpass = 0;

    result = vkCreateGraphicsPipelines(_device, _pipelineCache, 1, &pipeline, NULL, &_pipeline);
    assert(result == VK_SUCCESS);
}

void vulkan::initViewport()
{
    VkViewport viewport = {};
    viewport.height = (float)_width;
    viewport.width = (float)_height;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);
}

void vulkan::initScissor()
{
    VkRect2D scissor = {};
    scissor.extent.width = _width;
    scissor.extent.height = _height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void vulkan::setFrameUniforms(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    auto mvp = projectionMatrix * viewMatrix;
    memcpy(_uniformDataBuffer, &mvp, sizeof(glm::mat4));
}

void vulkan::buildCommandBuffer()
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = NULL;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = NULL;

    auto result = vkBeginCommandBuffer(_commandBuffer, &commandBufferBeginInfo);
    assert(result == VK_SUCCESS);

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.9f;
    clearValues[0].color.float32[1] = 0.9f;
    clearValues[0].color.float32[2] = 0.9f;
    clearValues[0].color.float32[3] = 0.9f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = NULL;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    result = vkCreateSemaphore(_device, &presentCompleteSemaphoreCreateInfo, NULL, &_presentCompleteSemaphore);
    assert(result == VK_SUCCESS);

    // Get the index of the next available swapchain image:
    result = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _presentCompleteSemaphore, NULL, &_currentBuffer);
    assert(result == VK_SUCCESS);

    VkRenderPassBeginInfo rprenderPassBeginInfo;
    rprenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rprenderPassBeginInfo.pNext = NULL;
    rprenderPassBeginInfo.renderPass = _renderPass;
    rprenderPassBeginInfo.framebuffer = _frameBuffers[_currentBuffer];
    rprenderPassBeginInfo.renderArea.offset.x = 0;
    rprenderPassBeginInfo.renderArea.offset.y = 0;
    rprenderPassBeginInfo.renderArea.extent.width = _width;
    rprenderPassBeginInfo.renderArea.extent.height = _height;
    rprenderPassBeginInfo.clearValueCount = 2;
    rprenderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(_commandBuffer, &rprenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, NUM_DESCRIPTOR_SETS, _descriptorSets, 0, NULL);

    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(_commandBuffer, 0, 1, &_vertexBuffer.buf, offsets);

    initViewport();
    initScissor();

    vkCmdDraw(_commandBuffer, 36, 1, 0, 0);
    vkCmdEndRenderPass(_commandBuffer);

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = NULL;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.image = _swapchainBuffers[_currentBuffer].image;
    vkCmdPipelineBarrier(_commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);

    result = vkEndCommandBuffer(_commandBuffer);
    assert(result == VK_SUCCESS);
}

void vulkan::draw()
{
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(_device, &fenceInfo, NULL, &_drawFence);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo;
    submitInfo.pNext = NULL;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &_presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;

    /* Queue the command buffer for execution */
    auto result = vkQueueSubmit(_queue, 1, &submitInfo, _drawFence);
    assert(result == VK_SUCCESS);

    /* Now present the image in the window */
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &_swapchain;
    present.pImageIndices = &_currentBuffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;

    /* Make sure command buffer is finished before presenting */
    do
    {
        result = vkWaitForFences(_device, 1, &_drawFence, VK_TRUE, FENCE_NANOSECONDS_TIMEOUT);
    } while (result == VK_TIMEOUT);

    assert(result == VK_SUCCESS);
    result = vkQueuePresentKHR(_queue, &present);
    assert(result == VK_SUCCESS);
}