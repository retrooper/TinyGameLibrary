#pragma once
#include "Mesh.h"
#include "Material.h"
namespace tgl {
    struct RenderObject {
        Mesh mesh;
        Material material;
        glm::mat4 transformationMatrix;
    };
}