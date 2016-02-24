#pragma once

#include "window.h"

#include <glm\glm.hpp>
#include <glslang\Include\ResourceLimits.h>
#include <glslang\Public\ShaderLang.h>
#include <vulkan\vulkan.h>

#include <vector>
#include <Windows.h>

class vulkan
{
    typedef struct
    {
        VkLayerProperties properties;
        std::vector<VkExtensionProperties> extensions;
    } layerProperties;

    typedef struct
    {
        VkImage image;
        VkImageView view;
    } swapchainBuffer;

    typedef struct
    {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    typedef struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } buffer;
private:
    const int FENCE_NANOSECONDS_TIMEOUT = 100000000;
    const int NUM_DESCRIPTOR_SETS = 1;

    HWND _windowHandle;
    HINSTANCE _windowHInstance;
    int _width;
    int _height;

    std::string _vertexShader;
    std::string _fragmentShader;

    VkInstance _vkInstance;
    VkSurfaceKHR _surface;
    VkPhysicalDevice _gpu;

    uint32_t _queueCount;

    VkQueueFamilyProperties* _queueProperties;
    VkPhysicalDeviceProperties* _deviceProperties;
    VkPhysicalDeviceMemoryProperties* _memoryProperties;

    uint32_t _graphicsQueueFamilyIndex;
    VkFormat _format;
    VkDevice _device;
    VkCommandPool _commandPool;
    VkCommandBuffer _commandBuffer;
    VkQueue _queue;
    VkSwapchainKHR _swapchain;
    uint32_t _swapchainImageCount;
    uint32_t _currentBuffer;
    std::vector<swapchainBuffer> _swapchainBuffers;
    depth _depth;
    buffer _uniformData;
    VkPipelineLayout _pipelineLayout;
    VkDescriptorSetLayout* _descriptorLayouts;
    VkRenderPass _renderPass;
    VkPipelineShaderStageCreateInfo _shaderStages[2];
    VkFramebuffer* _frameBuffers;
    buffer _vertexBuffer;
    VkVertexInputBindingDescription _vertexInputBindingDescription;
    VkVertexInputAttributeDescription _vertexInputAttributesDescription[2];
    VkDescriptorPool _descriptorPool;
    VkDescriptorSet* _descriptorSets;
    VkPipelineCache _pipelineCache;
    VkPipeline _pipeline;
    VkFence _drawFence;
    VkSemaphore _presentCompleteSemaphore;

    PFN_vkCreateDebugReportCallbackEXT _createDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT _destroyDebugReportCallback;
    VkDebugReportCallbackEXT _debugReportCallback;

    std::vector<char*> _enabledLayers;

    uint8_t* _uniformDataBuffer;
private:
    void initGlobalLayerProperties();
    void initGlobalExtensionProperties(layerProperties &layerProps);
    void createInstance();
    void initEnumerateDevice();
    void initSwapchainExtension();
    void createDevice();
    void registerDebugCallback();
    void createCommandPool();
    void createCommandBuffer();
    void createDeviceQueue();
    void initSwapChain();
    void initDepthBuffer();
    void initUniformBuffer();
    void createDescriptorAndPipelineLayouts();
    void createRenderpass();
    void initShaders();
    void initFramebuffers();
    void initVertexBuffer();
    void createDescriptorPool();
    void createDescriptorSet();
    void createPipelineCache();
    void createPipeline();
    void initViewport();
    void initScissor();
public:
    vulkan(HWND handle, HINSTANCE hInstance, int width, int height);
    ~vulkan();

    void setFrameUniforms(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    void buildCommandBuffer();
    void draw();
};