#pragma once

#include <vulkan\vulkan.h>

#include<vector>

class vkInitializers
{
public:
    vkInitializers();
    ~vkInitializers();

    static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescription,
        std::vector<VkVertexInputAttributeDescription>& vertexInputAttributesDescription)
    {
        VkPipelineVertexInputStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescription.size());
        info.pVertexBindingDescriptions = vertexInputBindingDescription.data();
        info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributesDescription.size());
        info.pVertexAttributeDescriptions = vertexInputAttributesDescription.data();

        return info;
    }

    static VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo()
    {
        VkPipelineInputAssemblyStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.primitiveRestartEnable = VK_FALSE;
        info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        return info;
    }

    static VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo()
    {
        VkPipelineRasterizationStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.polygonMode = VK_POLYGON_MODE_FILL;
        info.cullMode = VK_CULL_MODE_BACK_BIT;
        info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        info.depthClampEnable = true;
        info.rasterizerDiscardEnable = VK_FALSE;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0;
        info.depthBiasClamp = 0;
        info.depthBiasSlopeFactor = 0;
        info.lineWidth = 0;

        return info;
    }

    static VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState info = {};
        info.colorWriteMask = 0xf;
        info.blendEnable = VK_FALSE;
        info.alphaBlendOp = VK_BLEND_OP_ADD;
        info.colorBlendOp = VK_BLEND_OP_ADD;
        info.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

        return info;
    }

    static VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState>& attachments)
    {
        VkPipelineColorBlendStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.flags = 0;
        info.pNext = NULL;
        info.attachmentCount = static_cast<uint32_t>(attachments.size());
        info.pAttachments = attachments.data();
        info.logicOpEnable = VK_FALSE;
        info.logicOp = VK_LOGIC_OP_NO_OP;
        info.blendConstants[0] = 1.0f;
        info.blendConstants[1] = 1.0f;
        info.blendConstants[2] = 1.0f;
        info.blendConstants[3] = 1.0f;

        return info;
    }

    static VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount)
    {
        VkPipelineViewportStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.viewportCount = viewportCount;
        info.scissorCount = scissorCount;
        info.flags = 0;
        info.pScissors = NULL;
        info.pViewports = NULL;

        return info;
    }

    static VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo()
    {
        VkPipelineDepthStencilStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.depthTestEnable = true;
        info.depthWriteEnable = true;
        info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        info.depthBoundsTestEnable = VK_FALSE;
        info.stencilTestEnable = VK_FALSE;
        info.back.failOp = VK_STENCIL_OP_KEEP;
        info.back.passOp = VK_STENCIL_OP_KEEP;
        info.back.compareOp = VK_COMPARE_OP_ALWAYS;
        info.back.compareMask = 0;
        info.back.reference = 0;
        info.back.depthFailOp = VK_STENCIL_OP_KEEP;
        info.back.writeMask = 0;
        info.minDepthBounds = 0;
        info.maxDepthBounds = 0;
        info.stencilTestEnable = VK_FALSE;
        info.front = info.back;

        return info;
    }

    static VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.pSampleMask = NULL;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        info.sampleShadingEnable = VK_FALSE;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;
        info.minSampleShading = 0.0;

        return info;
    }

    static VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(std::vector<VkDynamicState>& enabledDynamicStates)
    {
        VkPipelineDynamicStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.pNext = NULL;
        info.pDynamicStates = enabledDynamicStates.data();
        info.dynamicStateCount = static_cast<uint32_t>(enabledDynamicStates.size());

        return info;
    }

    static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        VkPipelineLayout& pipelineLayout,
        VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo,
        VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo,
        VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo,
        VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo,
        VkPipelineMultisampleStateCreateInfo& multiSampleStateCreateInfo,
        VkPipelineDynamicStateCreateInfo& dynamicStateCreateInfo,
        VkPipelineViewportStateCreateInfo& viewportStateCreateInfo,
        VkPipelineDepthStencilStateCreateInfo& depthStateStencilCreateInfo,
        std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
        VkRenderPass& renderPass)
    {
        VkGraphicsPipelineCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = NULL;
        info.basePipelineHandle = VK_NULL_HANDLE;
        info.basePipelineIndex = 0;
        info.flags = 0;
        info.layout = pipelineLayout;
        info.pVertexInputState = &vertexInputStateCreateInfo;
        info.pInputAssemblyState = &inputAssemblyCreateInfo;
        info.pRasterizationState = &rasterizationStateCreateInfo;
        info.pColorBlendState = &colorBlendStateCreateInfo;
        info.pTessellationState = NULL;
        info.pMultisampleState = &multiSampleStateCreateInfo;
        info.pDynamicState = &dynamicStateCreateInfo;
        info.pViewportState = &viewportStateCreateInfo;
        info.pDepthStencilState = &depthStateStencilCreateInfo;
        info.stageCount = static_cast<uint32_t>(shaderStages.size());
        info.pStages = shaderStages.data();
        info.renderPass = renderPass;
        info.subpass = 0;

        return info;
    }
};

