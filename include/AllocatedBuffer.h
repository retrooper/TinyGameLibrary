#pragma once
#include "VMAIncluder.h"
namespace tgl {
    struct AllocatedBuffer {
        VkBuffer vkBuffer;
        VmaAllocation allocation;
    };
}