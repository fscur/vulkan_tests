#pragma once

#include "window.h"

#include <glm\glm.hpp>
#include <glslang\Include\ResourceLimits.h>
#include <glslang\Public\ShaderLang.h>
#include <vulkan\vulkan.h>

#include <vector>

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
    } uniformData;

    typedef struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } vertexBuffer;

private:
    const int FENCE_NANOSECONDS_TIMEOUT = 100000000;
    const int NUM_DESCRIPTOR_SETS = 1;

    window* _window;

    std::string _vertexShader;
    std::string _fragmentShader;

    VkInstance _vkInstance;
    std::vector<layerProperties> _instanceLayerProperties;
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
    uniformData _uniformData;
    VkPipelineLayout _pipelineLayout;
    VkDescriptorSetLayout* _descriptorLayouts;
    VkRenderPass _renderPass;
    VkPipelineShaderStageCreateInfo _shaderStages[2];
    VkFramebuffer* _frameBuffers;
    vertexBuffer _vertexBuffer;
    VkVertexInputBindingDescription _vertexInputBindingDescription;
    VkVertexInputAttributeDescription _vertexInputAttributesDescription[2];
    VkDescriptorPool _descriptorPool;
    VkDescriptorSet* _descriptorSets;
    VkPipelineCache _pipelineCache;
    VkPipeline _pipeline;
    VkViewport _viewport;
    VkRect2D _scissor;
    VkFence _drawFence;
    VkSemaphore _presentCompleteSemaphore;

    PFN_vkCreateDebugReportCallbackEXT _createDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT _destroyDebugReportCallback;
    PFN_vkDebugReportMessageEXT _debugReportMessdage;
    VkDebugReportCallbackEXT _debugReportCallback;

    std::vector<char*> _enabledLayers;

    glm::mat4 _projection;
    glm::mat4 _view;
    glm::mat4 _model;
    glm::mat4 _mvp;
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
    void buildCommandBuffer();
    void draw();
    void initViewport();
    void initScissor();
public:
    vulkan(window* window);
    ~vulkan();
};