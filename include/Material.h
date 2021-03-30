#pragma once
#include <vulkan/vulkan.h>
namespace tgl {
    struct Material {
        VkPipeline vkPipeline;
        VkPipelineLayout vkPipelineLayout;
    };
}