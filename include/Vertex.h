#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace tgl {
    struct VertexInputDescription {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        Vertex() = default;
        Vertex(glm::vec3 position, glm::vec3 normal, glm::vec4 color);

        bool operator==(const Vertex &b) const;

        bool operator!=(const Vertex &b) const;

        static VertexInputDescription getVertexDescription();
    };
}
namespace std {
    template<>
    struct hash<tgl::Vertex> {
        size_t operator()(tgl::Vertex const other) const {
            size_t posHash = hash<glm::vec3>()(other.position);
            size_t colorHash = hash<glm::vec4>()(other.color);
            //size_t uvHash = hash<glm::vec2>()(other.);
            size_t uvHash = 1;
            size_t normalHash = hash<glm::vec3>()(other.normal);
            return ((((posHash ^ (colorHash << 1)) >> 1) ^ uvHash) << 1) ^ normalHash;
        }
    };
}