#pragma once
#include "Vertex.h"
#include "AllocatedBuffer.h"
#include "MeshRenderData.h"
#include "PipelineBuilder.h"
#include <vector>
namespace tgl {
    struct MeshDescription {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        void* renderDataMappedDestination;
        VkDescriptorSet vkDescriptorSet;
        MeshRenderData renderData;
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer indexBuffer;
        AllocatedBuffer renderDataBuffer;

        bool operator==(const MeshDescription& other) const;
        bool operator<(const MeshDescription& other) const;
    };

    struct Mesh {
        MeshDescription description;
    };
}