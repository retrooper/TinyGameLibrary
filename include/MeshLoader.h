#pragma once
#include "Mesh.h"
#include "tiny_obj_loader.h"
#include "VkUtils.h"
#include <unordered_map>
#include <iostream>

namespace tgl {
    class MeshLoader {
    public:
        static Mesh loadObj(const char* filePath);
        static Mesh loadObj(const char* filePath, glm::vec4 color);
    };
}