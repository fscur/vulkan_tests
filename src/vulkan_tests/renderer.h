#pragma once

#include "window.h"
#include "vulkanDevice.h"
#include "vulkanPipeline.h"
#include "vulkanDebugger.h"
#include "mesh.h"

#include <glm\glm.hpp>
#include <glslang\Include\ResourceLimits.h>
#include <glslang\Public\ShaderLang.h>
#include <vulkan\vulkan.h>

#include <vector>
#include <Windows.h>

class renderer
{
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
private:
    const int FENCE_NANOSECONDS_TIMEOUT = 100000000;

    HWND _windowHandle;
    HINSTANCE _windowHInstance;
    int _width;
    int _height;

    std::vector<char*> _enabledLayers;
    std::vector<char*> _enabledExtensions;
    vulkanDebugger* _debugger;
    VkInstance _vkInstance;
    vulkanDevice* _device;
    vulkanPipeline* _pipeline;
    std::vector<mesh*> _renderList;



    VkSwapchainKHR _swapchain;
    uint32_t _swapchainImageCount;
    uint32_t _currentBuffer;
    std::vector<swapchainBuffer> _swapchainBuffers;
    depth _depth;
    VkFramebuffer* _frameBuffers;
    VkFence _drawFence;
    VkSemaphore _presentCompleteSemaphore;
private:
    void createInstance();

    void initSwapChain();
    void initDepthBuffer();
    void initFramebuffers();
public:
    renderer(HWND handle, HINSTANCE hInstance, int width, int height);
    ~renderer();

    void setFrameUniforms(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    void buildCommandBuffer();
    void render();
    void addObject(mesh* mesh);
};