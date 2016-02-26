#pragma once

#include "vulkanDevice.h"
#include "mesh.h"

#include <vulkan\vulkan.h>
#include <glm\glm.hpp>

#include <vector>

class vulkanPipeline
{
    typedef struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } buffer;

private:
    void* _uniformDataBuffer;

    vulkanDevice* _device;
public:
    const int NUM_DESCRIPTOR_SETS = 1;

    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributesDescription;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet* descriptorSets;
    VkDescriptorSetLayout* descriptorLayouts;

    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkRenderPass renderPass;

    VkPipelineLayout pipelineLayout;
    VkPipelineCache pipelineCache;
    VkPipeline vkPipeline;

    buffer uniformData;
    buffer vertexBuffer;
private:
    void createUniformBuffer();
    void createRenderpass();
    void createShaders(std::string vertexShader, std::string fragmentShader);

    void createVertexBuffer();
    void createDescriptorAndPipelineLayouts();
    void createDescriptorPool();
    void createDescriptorSet();
    void createPipelineCache();
    void createPipeline();
public:
    vulkanPipeline(vulkanDevice* vulkanDevice);
    ~vulkanPipeline();

    void updateFrameUniformsBuffer(glm::mat4 mvp);
    void updateVertexBuffer(std::vector<mesh*>& renderList);
};