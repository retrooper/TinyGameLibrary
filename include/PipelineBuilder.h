#pragma once

#include "VkUtils.h"
#include "Camera.h"
#include "Vertex.h"
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
        VkDescriptorPool vkDescriptorPool{};
        VkDescriptorSetLayout vkDescriptorSetLayout{};

        PipelineBuilder() = default;

        VkPipeline build(VkDevice &device, GPU& gpu, VkRenderPass &pass, VkShaderModule &vkVertexShaderModule,
                         VkShaderModule &vkFragmentShaderModule, VkViewport& vkViewport, VkRect2D& vkScissor, VkPrimitiveTopology vkTopology,
                         VkPolygonMode vkPolygonMode,
                         VkCullModeFlags vkCullModeFlags, VkFrontFace vkFrontFace, bool depthTestEnabled, bool depthWriteEnabled);

        void allocateDescriptorSets(VkDevice& vkLogicalDevice, VkDescriptorSet* vkDescriptorSet);
    };
}