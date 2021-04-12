#include "PipelineBuilder.h"

namespace tgl {
    VkPipeline PipelineBuilder::build(VkDevice &vkLogicalDevice, GPU& gpu, VkRenderPass &vkRenderPass, VkShaderModule &vkVertexShaderModule,
            VkShaderModule &vkFragmentShaderModule, VkViewport& vkViewport, VkRect2D& vkScissor, VkPrimitiveTopology vkTopology,
    VkPolygonMode vkPolygonMode,
            VkCullModeFlags vkCullModeFlags, VkFrontFace vkFrontFace, bool depthTestEnable, bool depthWriteEnable) {
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
        vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VkUtils::getMaxUsableSampleCount(gpu);
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
        vkPushConstantRange.size = sizeof(CameraData);
        vkPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//only accessible in the vertex shader

        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding;
        vkDescriptorSetLayoutBinding.binding = 0; //Binding we specified in the shader
        vkDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vkDescriptorSetLayoutBinding.descriptorCount = 1;
        vkDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //Use in our vertex shader
        vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;


        VkDescriptorPoolSize vkDescriptorPoolSize;
        vkDescriptorPoolSize.descriptorCount = 1;
        vkDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        VkDescriptorPoolSize vkSamplerPoolSize;
        vkSamplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        vkSamplerPoolSize.descriptorCount = 1;

        std::vector<VkDescriptorPoolSize> vkDescriptorPoolSizes;
        vkDescriptorPoolSizes.reserve(2);
        vkDescriptorPoolSizes.push_back(vkDescriptorPoolSize);
        vkDescriptorPoolSizes.push_back(vkSamplerPoolSize);

        VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo;
        vkDescriptorPoolCreateInfo.pNext = nullptr;
        vkDescriptorPoolCreateInfo.flags = 0;
        vkDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        vkDescriptorPoolCreateInfo.maxSets = 1; //One set
        vkDescriptorPoolCreateInfo.poolSizeCount = vkDescriptorPoolSizes.size();
        vkDescriptorPoolCreateInfo.pPoolSizes = vkDescriptorPoolSizes.data();

        VK_HANDLE_ERROR(
                vkCreateDescriptorPool(vkLogicalDevice, &vkDescriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
                "Failed to create a descriptor pool!");

        VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo;
        vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        vkDescriptorSetLayoutCreateInfo.pNext = nullptr;
        vkDescriptorSetLayoutCreateInfo.flags = 0;
        vkDescriptorSetLayoutCreateInfo.bindingCount = 1;
        vkDescriptorSetLayoutCreateInfo.pBindings = &vkDescriptorSetLayoutBinding;

        VK_HANDLE_ERROR(vkCreateDescriptorSetLayout(vkLogicalDevice, &vkDescriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout),
                        "Failed to create a descriptor set layout!");

        vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        vkDescriptorSetAllocateInfo.pNext = nullptr;
        vkDescriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
        vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
        vkDescriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;

        VK_HANDLE_ERROR(
                vkAllocateDescriptorSets(vkLogicalDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet),
                "Failed to allocate a descriptor set!");
        //TOODO vk write descriptorset


        VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{};
        vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vkPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        vkPipelineLayoutCreateInfo.pPushConstantRanges = &vkPushConstantRange;
        vkPipelineLayoutCreateInfo.setLayoutCount = 1;
        vkPipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayout;

        VK_HANDLE_ERROR(vkCreatePipelineLayout(vkLogicalDevice, &vkPipelineLayoutCreateInfo, nullptr, &vkPipelineLayout), "Failed to create a pipeline layout!");



        VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo{};
        vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        vkPipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable ? VK_TRUE : VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable ? VK_TRUE : VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.depthCompareOp = depthTestEnable ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_ALWAYS;
        vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
        vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
        vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
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
        vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;

        VkPipeline vkPipeline;
        VK_HANDLE_ERROR(vkCreateGraphicsPipelines(vkLogicalDevice, VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &vkPipeline), "Failed to create the graphics pipeline!");
        return vkPipeline;
    }
}