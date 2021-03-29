#pragma once

#include "VkUtils.h"
#include "Mesh.h"
namespace tgl {
    class PipelineBuilder {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> vkShaderStages;
        VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo{};
        VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo{};
        VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState{};
        VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo{};
        VkPipelineLayout vkPipelineLayout{};

        PipelineBuilder() = default;

        VkPipeline build(VkDevice &device, VkRenderPass &pass, VkShaderModule &vkVertexShaderModule,
                         VkShaderModule &vkFragmentShaderModule, VkViewport& vkViewport, VkRect2D& vkScissor, VkPrimitiveTopology vkTopology,
                         VkPolygonMode vkPolygonMode,
                         VkCullModeFlags vkCullModeFlags, VkFrontFace vkFrontFace);
    };
}