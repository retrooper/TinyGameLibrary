#pragma once
#include "Vertex.h"
#include "AllocatedBuffer.h"
#include "MeshData.h"
#include <vector>
namespace tgl {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        MeshData data;
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer indexBuffer;
        AllocatedBuffer meshDataBuffer;
        volatile bool uploaded = false;
    };
}