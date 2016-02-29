#pragma once

#include <vulkan\vulkan.h>
#include <glslang\Include\ResourceLimits.h>
#include <glslang\Public\ShaderLang.h>
#include <spirv\GlslangToSpv.h>

#include <vector>

class glslCompiler
{
private:
    glslCompiler(){ };

    static EShLanguage getLanguage(const VkShaderStageFlagBits shaderType)
    {
        switch (shaderType)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;

        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;

        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;

        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;

        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;

        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;

        default:
            return EShLangVertex;
        }
    }

    static TBuiltInResource createResources()
    {
        TBuiltInResource resources;
        resources.maxLights = 32;
        resources.maxClipPlanes = 6;
        resources.maxTextureUnits = 32;
        resources.maxTextureCoords = 32;
        resources.maxVertexAttribs = 64;
        resources.maxVertexUniformComponents = 4096;
        resources.maxVaryingFloats = 64;
        resources.maxVertexTextureImageUnits = 32;
        resources.maxCombinedTextureImageUnits = 80;
        resources.maxTextureImageUnits = 32;
        resources.maxFragmentUniformComponents = 4096;
        resources.maxDrawBuffers = 32;
        resources.maxVertexUniformVectors = 128;
        resources.maxVaryingVectors = 8;
        resources.maxFragmentUniformVectors = 16;
        resources.maxVertexOutputVectors = 16;
        resources.maxFragmentInputVectors = 15;
        resources.minProgramTexelOffset = -8;
        resources.maxProgramTexelOffset = 7;
        resources.maxClipDistances = 8;
        resources.maxComputeWorkGroupCountX = 65535;
        resources.maxComputeWorkGroupCountY = 65535;
        resources.maxComputeWorkGroupCountZ = 65535;
        resources.maxComputeWorkGroupSizeX = 1024;
        resources.maxComputeWorkGroupSizeY = 1024;
        resources.maxComputeWorkGroupSizeZ = 64;
        resources.maxComputeUniformComponents = 1024;
        resources.maxComputeTextureImageUnits = 16;
        resources.maxComputeImageUniforms = 8;
        resources.maxComputeAtomicCounters = 8;
        resources.maxComputeAtomicCounterBuffers = 1;
        resources.maxVaryingComponents = 60;
        resources.maxVertexOutputComponents = 64;
        resources.maxGeometryInputComponents = 64;
        resources.maxGeometryOutputComponents = 128;
        resources.maxFragmentInputComponents = 128;
        resources.maxImageUnits = 8;
        resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
        resources.maxCombinedShaderOutputResources = 8;
        resources.maxImageSamples = 0;
        resources.maxVertexImageUniforms = 0;
        resources.maxTessControlImageUniforms = 0;
        resources.maxTessEvaluationImageUniforms = 0;
        resources.maxGeometryImageUniforms = 0;
        resources.maxFragmentImageUniforms = 8;
        resources.maxCombinedImageUniforms = 8;
        resources.maxGeometryTextureImageUnits = 16;
        resources.maxGeometryOutputVertices = 256;
        resources.maxGeometryTotalOutputComponents = 1024;
        resources.maxGeometryUniformComponents = 1024;
        resources.maxGeometryVaryingComponents = 64;
        resources.maxTessControlInputComponents = 128;
        resources.maxTessControlOutputComponents = 128;
        resources.maxTessControlTextureImageUnits = 16;
        resources.maxTessControlUniformComponents = 1024;
        resources.maxTessControlTotalOutputComponents = 4096;
        resources.maxTessEvaluationInputComponents = 128;
        resources.maxTessEvaluationOutputComponents = 128;
        resources.maxTessEvaluationTextureImageUnits = 16;
        resources.maxTessEvaluationUniformComponents = 1024;
        resources.maxTessPatchComponents = 120;
        resources.maxPatchVertices = 32;
        resources.maxTessGenLevel = 64;
        resources.maxViewports = 16;
        resources.maxVertexAtomicCounters = 0;
        resources.maxTessControlAtomicCounters = 0;
        resources.maxTessEvaluationAtomicCounters = 0;
        resources.maxGeometryAtomicCounters = 0;
        resources.maxFragmentAtomicCounters = 8;
        resources.maxCombinedAtomicCounters = 8;
        resources.maxAtomicCounterBindings = 1;
        resources.maxVertexAtomicCounterBuffers = 0;
        resources.maxTessControlAtomicCounterBuffers = 0;
        resources.maxTessEvaluationAtomicCounterBuffers = 0;
        resources.maxGeometryAtomicCounterBuffers = 0;
        resources.maxFragmentAtomicCounterBuffers = 1;
        resources.maxCombinedAtomicCounterBuffers = 1;
        resources.maxAtomicCounterBufferSize = 16384;
        resources.maxTransformFeedbackBuffers = 4;
        resources.maxTransformFeedbackInterleavedComponents = 64;
        resources.maxCullDistances = 8;
        resources.maxCombinedClipAndCullDistances = 8;
        resources.maxSamples = 4;
        resources.limits.nonInductiveForLoops = 1;
        resources.limits.whileLoops = 1;
        resources.limits.doWhileLoops = 1;
        resources.limits.generalUniformIndexing = 1;
        resources.limits.generalAttributeMatrixVectorIndexing = 1;
        resources.limits.generalVaryingIndexing = 1;
        resources.limits.generalSamplerIndexing = 1;
        resources.limits.generalVariableIndexing = 1;
        resources.limits.generalConstantMatrixVectorIndexing = 1;

        return resources;
    }
public:
    static VkPipelineShaderStageCreateInfo buildShaderModuleFromGLSL(vulkanDevice* device, const VkShaderStageFlagBits shaderType, const std::string shader)
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo = {};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.pNext = NULL;
        shaderStageInfo.pSpecializationInfo = NULL;
        shaderStageInfo.flags = 0;
        shaderStageInfo.stage = shaderType;
        shaderStageInfo.pName = "main";

        auto spirvShader = glslCompiler::compile(shaderType, shader);

        VkShaderModuleCreateInfo shaderModule = {};
        shaderModule.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModule.pNext = NULL;
        shaderModule.flags = 0;
        shaderModule.codeSize = spirvShader.size() * sizeof(spirvShader[0]);
        shaderModule.pCode = spirvShader.data();

        auto result = vkCreateShaderModule(device->vkDevice, &shaderModule, NULL, &shaderStageInfo.module);
        assert(!result);

        return shaderStageInfo;
    }

    static std::vector<unsigned int> compile(const VkShaderStageFlagBits shaderType, const std::string shader)
    {
        glslang::InitializeProcess();

        auto language = getLanguage(shaderType);
        auto glslShader = new glslang::TShader(language);

        auto shaderInCharPointer = shader.c_str();
        glslShader->setStrings(&shaderInCharPointer, 1);

        // Enable SPIR-V and Vulkan rules when parsing GLSL
        auto messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
        auto resources = createResources();
        if (!glslShader->parse(&resources, 100, false, messages))
        {
            puts(glslShader->getInfoLog());
            puts(glslShader->getInfoDebugLog());
            assert(false);
        }

        auto program = glslang::TProgram();
        program.addShader(glslShader);
        if (!program.link(messages))
        {
            puts(glslShader->getInfoLog());
            puts(glslShader->getInfoDebugLog());
            assert(false);
        }

        std::vector<unsigned int> spirvShader;
        glslang::GlslangToSpv(*program.getIntermediate(language), spirvShader);
        glslang::FinalizeProcess();

        return spirvShader;
    }
};