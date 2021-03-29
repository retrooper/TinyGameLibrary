#pragma once
#include "Vertex.h"
#include "AllocatedBuffer.h"
#include <vector>
namespace tgl {
    struct Mesh {
        std::vector<Vertex> vertices;
        AllocatedBuffer allocatedBuffer;
    };
}