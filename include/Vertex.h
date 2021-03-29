#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
namespace tgl {
    struct VertexInputDescription {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags vkPipelineVertexInputStateCreateFlags = 0;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        static VertexInputDescription getVertexDescription();
    };
}