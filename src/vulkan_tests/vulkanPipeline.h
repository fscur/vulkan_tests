#pragma once

#include "vulkanDevice.h"
#include "vulkanBuffer.h"
#include "mesh.h"

#include <vulkan\vulkan.h>
#include <glm\glm.hpp>

#include <vector>

class vulkanPipeline
{
private:
    vulkanDevice* _device;

    std::vector<VkDescriptorSetLayoutBinding> _layoutBindings;
    std::vector<VkVertexInputBindingDescription> _vertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> _vertexInputAttributesDescription;

    VkDescriptorPool _descriptorPool;
    VkDescriptorSet _descriptorSet;
    VkDescriptorSetLayout _descriptorSetLayout;

    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

    VkPipelineLayout _pipelineLayout;
    VkPipelineCache _pipelineCache;
    VkPipeline _vkPipeline;

    vulkanBuffer* _frameUbo;
    vulkanBuffer* _verticesBuffer;
    vulkanBuffer* _indicesBuffer;
public:
    VkRenderPass renderPass;
private:
    void createVao();
    void createFrameUbo();
    void createRenderpass();
    void createShaderStages();
    void createDescriptorPool();
    void createDescriptorSetsLayout();
    void createDescriptorSets();
    void writeDescriptorSets();
    void createPipelineLayouts();
    void createPipelineCache();
    void createPipeline();
public:
    vulkanPipeline(vulkanDevice* vulkanDevice);
    ~vulkanPipeline();

    void bindTo(VkCommandBuffer & commandBuffer);
    void updateFrameUniformsBuffer(glm::mat4& vp);
    void updateVerticesBuffer(void* data, size_t dataSize);
    void updateIndicesBuffer(void* data, size_t dataSize);
};