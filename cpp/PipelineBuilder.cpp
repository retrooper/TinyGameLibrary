#include "PipelineBuilder.h"

namespace tgl {
    VkPipeline PipelineBuilder::build(VkDevice &vkLogicalDevice, VkRenderPass &vkRenderPass, VkShaderModule &vkVertexShaderModule,
            VkShaderModule &vkFragmentShaderModule, VkViewport& vkViewport, VkRect2D& vkScissor, VkPrimitiveTopology vkTopology,
    VkPolygonMode vkPolygonMode,
            VkCullModeFlags vkCullModeFlags, VkFrontFace vkFrontFace) {
        VkPipelineShaderStageCreateInfo vkPipelineShaderStageVertexCreateInfo{};
        vkPipelineShaderStageVertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkPipelineShaderStageVertexCreateInfo.pName = "main"; //Entry point
        vkPipelineShaderStageVertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vkPipelineShaderStageVertexCreateInfo.module = vkVertexShaderModule;

        VkPipelineShaderStageCreateInfo vkPipelineShaderStageFragmentCreateInfo{};
        vkPipelineShaderStageFragmentCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkPipelineShaderStageFragmentCreateInfo.pName = "main"; //Entry point
        vkPipelineShaderStageFragmentCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        vkPipelineShaderStageFragmentCreateInfo.module = vkFragmentShaderModule;

        vkShaderStages.push_back(vkPipelineShaderStageVertexCreateInfo);
        vkShaderStages.push_back(vkPipelineShaderStageFragmentCreateInfo);

        vkPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        auto vertexBindingDescriptions = Vertex::getVertexDescription().bindings;
        auto vertexAttributeDescriptions = Vertex::getVertexDescription().attributes;
        vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

        vkPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vkPipelineInputAssemblyStateCreateInfo.topology = vkTopology;
        vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.polygonMode = vkPolygonMode;
        vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0F;
        vkPipelineRasterizationStateCreateInfo.cullMode = vkCullModeFlags;
        vkPipelineRasterizationStateCreateInfo.frontFace = vkFrontFace;
        vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
        vkPipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
        vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

        vkPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        vkPipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        vkPipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
        vkPipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
        vkPipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        vkPipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        vkPipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                             VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        vkPipelineColorBlendAttachmentState.blendEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo{};
        vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vkPipelineViewportStateCreateInfo.scissorCount = 1;
        vkPipelineViewportStateCreateInfo.pScissors = &vkScissor;
        vkPipelineViewportStateCreateInfo.viewportCount = 1;
        vkPipelineViewportStateCreateInfo.pViewports = &vkViewport;

        VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo = {};
        vkPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        vkPipelineColorBlendStateCreateInfo.pNext = nullptr;
        vkPipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        vkPipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        vkPipelineColorBlendStateCreateInfo.attachmentCount = 1;
        vkPipelineColorBlendStateCreateInfo.pAttachments = &vkPipelineColorBlendAttachmentState;

        //Push constants
        VkPushConstantRange vkPushConstantRange{};
        vkPushConstantRange.offset = 0;
        vkPushConstantRange.size = sizeof(MeshPushConstants);
        vkPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//only accessible in the vertex shader
        VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{};
        vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vkPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        vkPipelineLayoutCreateInfo.pPushConstantRanges = &vkPushConstantRange;
        VK_HANDLE_ERROR(vkCreatePipelineLayout(vkLogicalDevice, &vkPipelineLayoutCreateInfo, nullptr, &vkPipelineLayout), "Failed to create a pipeline layout!");

        VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{};
        vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vkGraphicsPipelineCreateInfo.renderPass = vkRenderPass;
        vkGraphicsPipelineCreateInfo.subpass = 0;
        vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        vkGraphicsPipelineCreateInfo.stageCount = vkShaderStages.size();
        vkGraphicsPipelineCreateInfo.pStages = vkShaderStages.data();
        vkGraphicsPipelineCreateInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
        vkGraphicsPipelineCreateInfo.layout = vkPipelineLayout;

        VkPipeline vkPipeline;
        VK_HANDLE_ERROR(vkCreateGraphicsPipelines(vkLogicalDevice, VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &vkPipeline), "Failed to create the graphics pipeline!");
        return vkPipeline;
    }
}