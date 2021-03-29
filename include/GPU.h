#pragma once
#include <string>
#include <vector>
#include "vulkan/vulkan.h"
namespace tgl {
    enum GPUType {
        GPU_TYPE_OTHER = 0,
        GPU_TYPE_INTEGRATED = 1,
        GPU_TYPE_DISCRETE = 2,
        GPU_TYPE_VIRTUAL = 3,
        GPU_TYPE_CPU = 4
    };
    struct GPU {
        friend class Renderer;
        friend class VkUtils;
    public:
        std::string name;
        GPUType type;
        int vulkanAPIVersionSupported[3]{};//First index is major, second is minor third is patch
        int driverVersion{};
        int deviceID{};
    private:
        VkPhysicalDevice vkPhysicalDevice{};
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties{};
        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures{};
        VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties{};
        VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
        VkPhysicalDeviceLimits vkPhysicalDeviceLimits{};
    public:
        GPU() = default;
        GPU(VkPhysicalDevice &vkPhysicalDevice);
    };
}