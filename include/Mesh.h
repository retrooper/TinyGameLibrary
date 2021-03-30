#pragma once
#include "Vertex.h"
#include "AllocatedBuffer.h"
#include "MeshRenderData.h"
#include <vector>
namespace tgl {
    struct MeshDescription {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer indexBuffer;

        bool operator==(const MeshDescription& other) const;
        bool operator<(const MeshDescription& other) const;
    };

    struct Mesh {
        MeshDescription description;
        MeshRenderData data;
        AllocatedBuffer meshDataBuffer;
        volatile bool uploaded = false;
    };
}