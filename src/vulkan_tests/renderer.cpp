#include "renderer.h"

#include "glslCompiler.h"
#include "vulkanHelper.h"
#include "shaderLoader.h"
#include "vulkanDebugger.h"

#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <assert.h>
#include <vector>

renderer::renderer(HWND handle, HINSTANCE hInstance, int width, int height) :
    _windowHandle(handle),
    _windowHInstance(hInstance),
    _width(width),
    _height(height)
{
    _enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");

    _enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    _enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    _enabledExtensions.push_back("VK_EXT_debug_report");

    createInstance();
    vulkanDebugger::setupDebugger(_vkInstance);
    _device = new vulkanDevice(_vkInstance, handle, hInstance, _enabledLayers, _enabledExtensions);
    _pipeline = new vulkanPipeline(_device);

    initSwapChain();
    initDepthBuffer();
    initFramebuffers();
}

renderer::~renderer()
{
    vkDestroySemaphore(_device->vkDevice, _presentCompleteSemaphore, NULL);
    vkDestroyFence(_device->vkDevice, _drawFence, NULL);
    vkDestroyPipeline(_device->vkDevice, _pipeline->vkPipeline, NULL);
    vkDestroyPipelineCache(_device->vkDevice, _pipeline->pipelineCache, NULL);
    vkDestroyDescriptorPool(_device->vkDevice, _pipeline->descriptorPool, NULL);

    vkDestroyBuffer(_device->vkDevice, _pipeline->vertexBuffer.buf, NULL);
    vkFreeMemory(_device->vkDevice, _pipeline->vertexBuffer.mem, NULL);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        vkDestroyFramebuffer(_device->vkDevice, _frameBuffers[i], NULL);
    }
    free(_frameBuffers);

    vkDestroyShaderModule(_device->vkDevice, _pipeline->shaderStages[0].module, NULL);
    vkDestroyShaderModule(_device->vkDevice, _pipeline->shaderStages[1].module, NULL);

    vkDestroyRenderPass(_device->vkDevice, _pipeline->renderPass, NULL);

    for (int i = 0; i < _pipeline->NUM_DESCRIPTOR_SETS; i++)
    {
        vkDestroyDescriptorSetLayout(_device->vkDevice, _pipeline->descriptorLayouts[i], NULL);
    }

    vkDestroyPipelineLayout(_device->vkDevice, _pipeline->pipelineLayout, NULL);

    vkDestroyBuffer(_device->vkDevice, _pipeline->uniformData.buf, NULL);
    vkFreeMemory(_device->vkDevice, _pipeline->uniformData.mem, NULL);

    vkDestroyImageView(_device->vkDevice, _depth.view, NULL);
    vkDestroyImage(_device->vkDevice, _depth.image, NULL);
    vkFreeMemory(_device->vkDevice, _depth.mem, NULL);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        vkDestroyImageView(_device->vkDevice, _swapchainBuffers[i].view, NULL);
    }
    vkDestroySwapchainKHR(_device->vkDevice, _swapchain, NULL);

    vkFreeCommandBuffers(_device->vkDevice, _device->commandPool, 1, &_device->commandBuffer);
    vkDestroyCommandPool(_device->vkDevice, _device->commandPool, NULL);
    vkDestroyDevice(_device->vkDevice, NULL);
    vulkanDebugger::releaseDebugger(_vkInstance);
    vkDestroyInstance(_vkInstance, NULL);

    for (auto mesh : _renderList)
    {
        delete mesh;
    }
}

void renderer::createInstance()
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

    instanceInfo.enabledExtensionCount = _enabledExtensions.size();
    instanceInfo.ppEnabledExtensionNames = _enabledExtensions.data();

    VkResult result = vkCreateInstance(&instanceInfo, NULL, &_vkInstance);
    assert(result == VK_SUCCESS);
}

void renderer::initSwapChain()
{
    VkResult result;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device->primaryGpu, _device->surface, &surfaceCapabilities);
    assert(result == VK_SUCCESS);

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(_device->primaryGpu, _device->surface, &presentModeCount, NULL);
    assert(result == VK_SUCCESS);

    auto presentModes = new VkPresentModeKHR[presentModeCount];
    assert(presentModes);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(_device->primaryGpu, _device->surface, &presentModeCount, presentModes);
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
    swapChainCreateInfo.surface = _device->surface;
    swapChainCreateInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapChainCreateInfo.imageFormat = _device->surfaceFormat;
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

    result = vkCreateSwapchainKHR(_device->vkDevice, &swapChainCreateInfo, NULL, &_swapchain);
    assert(result == VK_SUCCESS);

    result = vkGetSwapchainImagesKHR(_device->vkDevice, _swapchain, &_swapchainImageCount, NULL);
    assert(result == VK_SUCCESS);

    auto swapchainImages = new VkImage[_swapchainImageCount];
    assert(swapchainImages);
    result = vkGetSwapchainImagesKHR(_device->vkDevice, _swapchain, &_swapchainImageCount, swapchainImages);
    assert(result == VK_SUCCESS);

    for (uint32_t i = 0; i < _swapchainImageCount; i++)
    {
        swapchainBuffer swapchaingBuffer;

        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = _device->surfaceFormat;
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
        //Produces warnings and I dont know what it does so im commenting it
        //vulkanHelper::setImageLayout(_device->commandBuffer, swapchaingBuffer.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        color_image_view.image = swapchaingBuffer.image;

        result = vkCreateImageView(_device->vkDevice, &color_image_view, NULL, &swapchaingBuffer.view);
        _swapchainBuffers.push_back(swapchaingBuffer);
        assert(result == VK_SUCCESS);
    }
    _currentBuffer = 0;

    if (NULL != presentModes)
    {
        free(presentModes);
    }
}

void renderer::initDepthBuffer()
{
    _depth.format = VK_FORMAT_D16_UNORM;

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(_device->primaryGpu, _depth.format, &formatProperties);

    VkImageCreateInfo imageCreateInfo = {};
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
        std::cout << "depth_format " << _depth.format << " Unsupported.\n";
        exit(-1);
    }

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = _depth.format;
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
    viewCreateInfo.format = _depth.format;
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

    /* Create image */
    auto result = vkCreateImage(_device->vkDevice, &imageCreateInfo, NULL, &_depth.image);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(_device->vkDevice, _depth.image, &memoryRequirements);

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    /* Use the memory properties to determine the type of memory required */
    auto pass = vulkanHelper::memoryTypeFromProperties(*_device->memoryProperties, memoryRequirements.memoryTypeBits, 0, &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    result = vkAllocateMemory(_device->vkDevice, &memoryAllocateInfo, NULL, &_depth.mem);
    assert(result == VK_SUCCESS);

    /* Bind memory */
    result = vkBindImageMemory(_device->vkDevice, _depth.image, _depth.mem, 0);
    assert(result == VK_SUCCESS);

    //Produces warnings and i dont know what it does so im commenting it
    /* Set the image layout to depth stencil optimal */
    //vulkanHelper::setImageLayout(_commandBuffer, _depth.image, viewCreateInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    /* Create image view */
    viewCreateInfo.image = _depth.image;
    result = vkCreateImageView(_device->vkDevice, &viewCreateInfo, NULL, &_depth.view);
    assert(result == VK_SUCCESS);
}

void renderer::initFramebuffers()
{
    VkResult result;
    VkImageView imageAttachments[2];
    imageAttachments[1] = _depth.view;

    VkFramebufferCreateInfo frameBufferCreateInfo;
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = _pipeline->renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = imageAttachments;
    frameBufferCreateInfo.width = _width;
    frameBufferCreateInfo.height = _height;
    frameBufferCreateInfo.layers = 1;

    _frameBuffers = new VkFramebuffer[_swapchainImageCount];

    for (auto i = 0; i < _swapchainImageCount; i++)
    {
        imageAttachments[0] = _swapchainBuffers[i].view;
        result = vkCreateFramebuffer(_device->vkDevice, &frameBufferCreateInfo, NULL, &_frameBuffers[i]);
        assert(result == VK_SUCCESS);
    }
}





void renderer::setFrameUniforms(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    _pipeline->updateFrameUniformsBuffer(projectionMatrix * viewMatrix);
}

void renderer::buildCommandBuffer()
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = NULL;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = NULL;

    auto result = vkBeginCommandBuffer(_device->commandBuffer, &commandBufferBeginInfo);
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

    result = vkCreateSemaphore(_device->vkDevice, &presentCompleteSemaphoreCreateInfo, NULL, &_presentCompleteSemaphore);
    assert(result == VK_SUCCESS);

    // Get the index of the next available swapchain image:
    result = vkAcquireNextImageKHR(_device->vkDevice, _swapchain, UINT64_MAX, _presentCompleteSemaphore, NULL, &_currentBuffer);
    assert(result == VK_SUCCESS);

    VkRenderPassBeginInfo rprenderPassBeginInfo;
    rprenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rprenderPassBeginInfo.pNext = NULL;
    rprenderPassBeginInfo.renderPass = _pipeline->renderPass;
    rprenderPassBeginInfo.framebuffer = _frameBuffers[_currentBuffer];
    rprenderPassBeginInfo.renderArea.offset.x = 0;
    rprenderPassBeginInfo.renderArea.offset.y = 0;
    rprenderPassBeginInfo.renderArea.extent.width = _width;
    rprenderPassBeginInfo.renderArea.extent.height = _height;
    rprenderPassBeginInfo.clearValueCount = 2;
    rprenderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(_device->commandBuffer, &rprenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)_width;
    viewport.width = (float)_height;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(_device->commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent.width = _width;
    scissor.extent.height = _height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(_device->commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(
        _device->commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _pipeline->pipelineLayout,
        0,
        _pipeline->NUM_DESCRIPTOR_SETS,
        _pipeline->descriptorSets,
        0, nullptr);

    vkCmdBindPipeline(_device->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->vkPipeline);

    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(_device->commandBuffer, 0, 1, &_pipeline->vertexBuffer.buf, offsets);

    vkCmdDraw(_device->commandBuffer, 36, 1, 0, 0);

    vkCmdEndRenderPass(_device->commandBuffer);

    VkImageMemoryBarrier* prePresentBarrier = new VkImageMemoryBarrier();
    prePresentBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier->pNext = NULL;
    prePresentBarrier->srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier->dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier->oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier->newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    prePresentBarrier->subresourceRange.baseMipLevel = 0;
    prePresentBarrier->subresourceRange.levelCount = 1;
    prePresentBarrier->subresourceRange.baseArrayLayer = 0;
    prePresentBarrier->subresourceRange.layerCount = 1;
    prePresentBarrier->image = _swapchainBuffers[_currentBuffer].image;

    vkCmdPipelineBarrier(
        _device->commandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, prePresentBarrier);

    result = vkEndCommandBuffer(_device->commandBuffer);
    assert(result == VK_SUCCESS);
}

void renderer::render()
{
    //VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    //presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    //presentCompleteSemaphoreCreateInfo.pNext = NULL;
    //presentCompleteSemaphoreCreateInfo.flags = 0;

    //auto result = vkCreateSemaphore(_device->vkDevice, &presentCompleteSemaphoreCreateInfo, NULL, &_presentCompleteSemaphore);
    //assert(result == VK_SUCCESS);

    //// Get the index of the next available swapchain image:
    //result = vkAcquireNextImageKHR(_device->vkDevice, _swapchain, UINT64_MAX, _presentCompleteSemaphore, NULL, &_currentBuffer);
    //assert(result == VK_SUCCESS);

    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(_device->vkDevice, &fenceInfo, NULL, &_drawFence);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &_presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_device->commandBuffer;

    auto result = vkQueueSubmit(_device->queue, 1, &submitInfo, _drawFence);
    assert(result == VK_SUCCESS);

    do
    {
        result = vkWaitForFences(_device->vkDevice, 1, &_drawFence, VK_TRUE, FENCE_NANOSECONDS_TIMEOUT);
    } while (result == VK_TIMEOUT);

    assert(result == VK_SUCCESS);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.pImageIndices = &_currentBuffer;
    presentInfo.pWaitSemaphores = NULL;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pResults = NULL;

    result = vkQueuePresentKHR(_device->queue, &presentInfo);
    assert(result == VK_SUCCESS);

    vkDestroySemaphore(_device->vkDevice, _presentCompleteSemaphore, nullptr);

    VkImageMemoryBarrier postPresentBarrier = {};
    postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    postPresentBarrier.pNext = NULL;
    postPresentBarrier.srcAccessMask = 0;
    postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    postPresentBarrier.image = _swapchainBuffers[_currentBuffer].image;

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    result= vkBeginCommandBuffer(_device->postPresentCommandBuffer, &cmdBufInfo);
    assert(!result);

    vkCmdPipelineBarrier(
        _device->postPresentCommandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &postPresentBarrier);

    result = vkEndCommandBuffer(_device->postPresentCommandBuffer);
    assert(!result);

    submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_device->postPresentCommandBuffer;

    result = vkQueueSubmit(_device->queue, 1, &submitInfo, VK_NULL_HANDLE);
    assert(!result);

    result = vkQueueWaitIdle(_device->queue);
    assert(!result);
}

void renderer::addObject(mesh* mesh)
{
    _renderList.push_back(mesh);
    _pipeline->updateVertexBuffer(_renderList);
}