#include "Vertex.h"
namespace tgl {
    VertexInputDescription Vertex::getVertexDescription() {
        //Position is at location 0, Normal at location 1, color at location 2
        VertexInputDescription vertexInputDescription;
        VkVertexInputBindingDescription mainBinding = {};
        mainBinding.binding = 0;
        mainBinding.stride = sizeof(Vertex);
        mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputDescription.bindings.push_back(mainBinding);
        VkVertexInputAttributeDescription positionAttribute = {};
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttribute.offset = offsetof(Vertex, position);
        VkVertexInputAttributeDescription normalAttribute = {};
        normalAttribute.binding = 0;
        normalAttribute.location = 1;
        normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        normalAttribute.offset = offsetof(Vertex, normal);
        VkVertexInputAttributeDescription colorAttribute = {};
        colorAttribute.binding = 0;
        colorAttribute.location = 2;
        colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        colorAttribute.offset = offsetof(Vertex, color);
        vertexInputDescription.attributes.push_back(positionAttribute);
        vertexInputDescription.attributes.push_back(normalAttribute);
        vertexInputDescription.attributes.push_back(colorAttribute);

        return vertexInputDescription;
    }
}