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
    uint32_t _gpusCount; // needed?
    uint32_t _queueCount; // needed?
    VkPhysicalDevice* _gpus;
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
    VkDescriptorSetLayout* _descriptorLayout;
    VkRenderPass _renderPass;
    VkPipelineShaderStageCreateInfo _shaderStages[2];
    VkFramebuffer* _frameBuffers;
    vertexBuffer _vertexBuffer;
    VkVertexInputBindingDescription _vertexInputBindingDescription;
    VkVertexInputAttributeDescription _vertexInputAttributesDescription[2];
    VkDescriptorPool _descriptorPool;
    VkDescriptorSet* _descriptorSet;
    VkPipelineCache _pipelineCache;
    VkPipeline _pipeline;
    VkViewport _viewport;
    VkRect2D _scissor;

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
    void initInstance();
    void initEnumerateDevice();
    void initSwapchainExtension();
    void initDevice();
    void registerDebugCallback();
    void initCommandPool();
    void initCommandBuffer();
    void executeBeginCommandBuffer();
    void initDeviceQueue();
    void initSwapChain();
    void initDepthBuffer();
    void initUniformBuffer();
    void initDescriptorAndPipelineLayouts();
    void initRenderpass();
    void initShaders();
    void initFramebuffers();
    void initVertexBuffer();
    void initDescriptorPool();
    void initDescriptorSet();
    void initPipelineCache();
    void initPipeline();
    void draw();
    void initViewport();
    void initScissor();
public:
    vulkan(window* window);
    ~vulkan();
};