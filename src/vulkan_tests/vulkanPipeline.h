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
public:
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributesDescription;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;

    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkRenderPass renderPass;

    VkPipelineLayout pipelineLayout;
    VkPipelineCache pipelineCache;
    VkPipeline vkPipeline;

    vulkanBuffer* frameUniformsUbo;
    vulkanBuffer* verticesBuffer;
    vulkanBuffer* indicesBuffer;
private:
    void createUniformBuffer();
    void createRenderpass();
    void createShaders(std::string vertexShader, std::string fragmentShader);

    void createVertexBuffer();
    void createPipelineLayouts();
    void createDescriptorPool();
    void createDescriptorSets();
    void createPipelineCache();
    void createPipeline();
    void updateDescriptorSets();
public:
    vulkanPipeline(vulkanDevice* vulkanDevice);
    ~vulkanPipeline();

    void bindTo(VkCommandBuffer & commandBuffer);
    void updateFrameUniformsBuffer(glm::mat4 mvp);
    void updateVertexBuffer(std::vector<mesh*>& renderList);
};