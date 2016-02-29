#include "vulkanPipeline.h"

#include "vulkanHelper.h"
#include "mesh.h"
#include "glslCompiler.h"
#include "shaderLoader.h"
#include "vkInitializers.h"
#include "vkTools.h"

#include <assert.h>

vulkanPipeline::vulkanPipeline(vulkanDevice* vulkanDevice) :
    _device(vulkanDevice)
{
    createVao();
    createFrameUbo();
    createRenderpass();
    createShaderStages();
    createDescriptorPool();
    createDescriptorSetsLayout();
    createDescriptorSets();
    createPipelineLayouts();
    createPipelineCache();
    createPipeline();
}

vulkanPipeline::~vulkanPipeline()
{
    vkDestroyDescriptorPool(_device->vkDevice, _descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(_device->vkDevice, _descriptorSetLayout, NULL);
    vkDestroyPipeline(_device->vkDevice, _vkPipeline, NULL);
    vkDestroyPipelineCache(_device->vkDevice, _pipelineCache, NULL);
    vkDestroyPipelineLayout(_device->vkDevice, _pipelineLayout, NULL);
    vkDestroyRenderPass(_device->vkDevice, renderPass, NULL);

    _indicesBuffer->~vulkanBuffer();
    _verticesBuffer->~vulkanBuffer();
    _frameUbo->~vulkanBuffer();

    for (auto& shaderStage : _shaderStages)
    {
        vkDestroyShaderModule(_device->vkDevice, shaderStage.module, NULL);
    }
}

void vulkanPipeline::createVao()
{
    auto cube = mesh::createCube();
    auto verticesDataSize = sizeof(vertex) * cube->vertices.size();
    auto indicesDataSize = sizeof(unsigned int) * cube->indices.size();

    //TODO: does vao need a better solution for starting up empty ?
    _indicesBuffer = new vulkanBuffer(_device, nullptr, 1, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    _verticesBuffer = new vulkanBuffer(_device, nullptr, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    VkVertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding = 0;
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBinding.stride = sizeof(vertex);

    _vertexInputBindingDescription.push_back(vertexInputBinding);

    VkVertexInputAttributeDescription verticesInputBindingDescription = {};
    verticesInputBindingDescription.binding = 0;
    verticesInputBindingDescription.location = 0;
    verticesInputBindingDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    verticesInputBindingDescription.offset = offsetof(vertex, vertex::position);

    VkVertexInputAttributeDescription texCoordsInputBindingDescription = {};
    texCoordsInputBindingDescription.binding = 0;
    texCoordsInputBindingDescription.location = 1;
    texCoordsInputBindingDescription.format = VK_FORMAT_R32G32_SFLOAT;
    texCoordsInputBindingDescription.offset = offsetof(vertex, vertex::texCoord);

    VkVertexInputAttributeDescription normalsInputBindingDescription = {};
    normalsInputBindingDescription.binding = 0;
    normalsInputBindingDescription.location = 2;
    normalsInputBindingDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalsInputBindingDescription.offset = offsetof(vertex, vertex::normal);

    _vertexInputAttributesDescription.push_back(verticesInputBindingDescription);
    _vertexInputAttributesDescription.push_back(texCoordsInputBindingDescription);
    _vertexInputAttributesDescription.push_back(normalsInputBindingDescription);
}

void vulkanPipeline::createFrameUbo()
{
    _frameUbo = new vulkanBuffer(_device, nullptr, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding;
    uniformBufferLayoutBinding.binding = 0;
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.descriptorCount = 1;
    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uniformBufferLayoutBinding.pImmutableSamplers = NULL;

    //if (use_texture) 
    //{
    //    layout_bindings[1].binding = 1;
    //    layout_bindings[1].descriptorType =
    //        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    layout_bindings[1].descriptorCount = 1;
    //    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    //    layout_bindings[1].pImmutableSamplers = NULL;
    //}

    _layoutBindings.push_back(uniformBufferLayoutBinding);
}

void vulkanPipeline::createRenderpass()
{
    VkFormat supportedDepthFormat;
    auto supportsDepthFormat = vkTools::getSupportedDepthFormat(_device->primaryGpu, supportedDepthFormat);
    assert(supportsDepthFormat);

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = _device->surfaceFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.flags = 0;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = supportedDepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.flags = 0;

    auto attachments = std::vector<VkAttachmentDescription>(2);
    auto colorAttachmentIndex = 0;
    auto depthAttachmentIndex = 1;
    attachments[colorAttachmentIndex] = colorAttachment;
    attachments[depthAttachmentIndex] = depthAttachment;

    VkAttachmentReference colorReference;
    colorReference.attachment = colorAttachmentIndex;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference;
    depthReference.attachment = depthAttachmentIndex;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = &depthReference; //TODO: fix check what this does with lots of cubes
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = NULL;

    auto result = vkCreateRenderPass(_device->vkDevice, &renderPassCreateInfo, NULL, &renderPass);
    assert(result == VK_SUCCESS);
}

void vulkanPipeline::createShaderStages()
{
    auto vertexShader = shaderLoader::load(R"(..\..\resources\shaders\basic.vert)");
    auto fragmentShader = shaderLoader::load(R"(..\..\resources\shaders\basic.frag)");

    assert(vertexShader != "");
    assert(fragmentShader != "");

    _shaderStages.push_back(glslCompiler::buildShaderModuleFromGLSL(_device, VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
    _shaderStages.push_back(glslCompiler::buildShaderModuleFromGLSL(_device, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void vulkanPipeline::createDescriptorPool()
{
    VkDescriptorPoolSize uniformBufferDescriptorPoolSize;
    uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferDescriptorPoolSize.descriptorCount = 1;

    //if (use_texture) 
    //{
    //    VkDescriptorPoolSize textureDescriptorPoolSize;
    //    textureDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    textureDescriptorPoolSize.descriptorCount = 1;
    //}

    std::vector<VkDescriptorPoolSize> poolSizes = { uniformBufferDescriptorPoolSize };

    VkDescriptorPoolCreateInfo descriptorPoolInfo;
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = NULL;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    auto result = vkCreateDescriptorPool(_device->vkDevice, &descriptorPoolInfo, NULL, &_descriptorPool);
    assert(result == VK_SUCCESS);
}

void vulkanPipeline::createDescriptorSetsLayout()
{
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.pNext = NULL;
    descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(_layoutBindings.size());
    descriptorLayoutInfo.pBindings = _layoutBindings.data();

    auto result = vkCreateDescriptorSetLayout(_device->vkDevice, &descriptorLayoutInfo, NULL, &_descriptorSetLayout);
    assert(!result);
}

void vulkanPipeline::createDescriptorSets()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = NULL;
    descriptorSetAllocateInfo.descriptorPool = _descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &_descriptorSetLayout;

    auto result = vkAllocateDescriptorSets(_device->vkDevice, &descriptorSetAllocateInfo, &_descriptorSet);
    assert(result == VK_SUCCESS);

    writeDescriptorSets();
}

void vulkanPipeline::writeDescriptorSets()
{
    VkWriteDescriptorSet uniformBufferDescriptorSetWrite = {};
    uniformBufferDescriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniformBufferDescriptorSetWrite.pNext = NULL;
    uniformBufferDescriptorSetWrite.dstSet = _descriptorSet;
    uniformBufferDescriptorSetWrite.descriptorCount = 1;
    uniformBufferDescriptorSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferDescriptorSetWrite.pBufferInfo = &_frameUbo->getBufferInfo();
    uniformBufferDescriptorSetWrite.dstArrayElement = 0;
    uniformBufferDescriptorSetWrite.dstBinding = 0;

    //if (use_texture)
    //{
    //    VkWriteDescriptorSet textureDescriptorSetWrite = {};
    //    textureDescriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //    textureDescriptorSetWrite.dstSet = info.desc_set[0];
    //    textureDescriptorSetWrite.dstBinding = 1;
    //    textureDescriptorSetWrite.descriptorCount = 1;
    //    textureDescriptorSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    textureDescriptorSetWrite.pImageInfo = &info.texture_data.image_info;
    //    textureDescriptorSetWrite.dstArrayElement = 0;
    //}

    std::vector<VkWriteDescriptorSet> descriptorSetWrites = { uniformBufferDescriptorSetWrite };

    vkUpdateDescriptorSets(_device->vkDevice, (uint32_t)descriptorSetWrites.size(), descriptorSetWrites.data(), 0, NULL);
}

void vulkanPipeline::createPipelineLayouts()
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = NULL;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &_descriptorSetLayout;

    auto result = vkCreatePipelineLayout(_device->vkDevice, &pipelineLayoutCreateInfo, NULL, &_pipelineLayout);
    assert(!result);
}

void vulkanPipeline::createPipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo;
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.pNext = NULL;
    pipelineCacheCreateInfo.initialDataSize = 0;
    pipelineCacheCreateInfo.pInitialData = NULL;
    pipelineCacheCreateInfo.flags = 0;

    auto result = vkCreatePipelineCache(_device->vkDevice, &pipelineCacheCreateInfo, NULL, &_pipelineCache);
    assert(result == VK_SUCCESS);
}

void vulkanPipeline::createPipeline()
{
    auto vertexInputStateCreateInfo = vkInitializers::pipelineVertexInputStateCreateInfo(_vertexInputBindingDescription, _vertexInputAttributesDescription);
    auto inputAssemblyCreateInfo = vkInitializers::pipelineInputAssemblyStateCreateInfo();
    auto rasterizationStateCreateInfo = vkInitializers::pipelineRasterizationStateCreateInfo();
    auto viewportStateCreateInfo = vkInitializers::pipelineViewportStateCreateInfo(1, 1);
    auto depthStateStencilCreateInfo = vkInitializers::pipelineDepthStencilStateCreateInfo();
    auto multiSampleStateCreateInfo = vkInitializers::pipelineMultisampleStateCreateInfo();
    auto colorBlendAttatchmentState = vkInitializers::pipelineColorBlendAttachmentState();

    auto attatchments = std::vector<VkPipelineColorBlendAttachmentState>{ colorBlendAttatchmentState };
    auto colorBlendStateCreateInfo = vkInitializers::pipelineColorBlendStateCreateInfo(attatchments);

    auto enabledDynamicStates = std::vector<VkDynamicState>{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    auto dynamicState = vkInitializers::pipelineDynamicStateCreateInfo(enabledDynamicStates);

    VkGraphicsPipelineCreateInfo pipeline = vkInitializers::graphicsPipelineCreateInfo(
        _pipelineLayout,
        vertexInputStateCreateInfo,
        inputAssemblyCreateInfo,
        rasterizationStateCreateInfo,
        colorBlendStateCreateInfo,
        multiSampleStateCreateInfo,
        dynamicState,
        viewportStateCreateInfo,
        depthStateStencilCreateInfo,
        _shaderStages,
        renderPass);

    auto result = vkCreateGraphicsPipelines(_device->vkDevice, _pipelineCache, 1, &pipeline, NULL, &_vkPipeline);
    assert(result == VK_SUCCESS);
}

void vulkanPipeline::updateFrameUniformsBuffer(glm::mat4& vp)
{
    //TODO: if ubo size changes must call writeDescriptorSets();
    _frameUbo->update(&vp, sizeof(glm::mat4));
}

void vulkanPipeline::updateVerticesBuffer(void* data, size_t dataSize)
{
    _verticesBuffer->update(data, dataSize);
}

void vulkanPipeline::updateIndicesBuffer(void* data, size_t dataSize)
{
    _indicesBuffer->update(data, dataSize);
}

void vulkanPipeline::bindTo(VkCommandBuffer& commandBuffer)
{
    auto descriptorSetsCount = 1;

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _pipelineLayout,
        0,
        descriptorSetsCount,
        &_descriptorSet,
        0, nullptr);

    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vkPipeline);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_verticesBuffer->buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _indicesBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
}