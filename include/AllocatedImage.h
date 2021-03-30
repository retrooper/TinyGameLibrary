#pragma once
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>
namespace tgl {
    struct AllocatedImage {
        VkImage image;
        VmaAllocation allocation;
    };
}