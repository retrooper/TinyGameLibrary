#include "GPU.h"

namespace tgl {
    GPU::GPU(VkPhysicalDevice &vkPhysicalDevice) {
        //Vulkan physical device data
        this->vkPhysicalDevice = vkPhysicalDevice;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &vkPhysicalDeviceMemoryProperties);
        this->vkPhysicalDeviceLimits = vkPhysicalDeviceProperties.limits;
        //Our data
        this->name = vkPhysicalDeviceProperties.deviceName;
        this->type = (GPUType) vkPhysicalDeviceProperties.deviceType;
        this->vulkanAPIVersionSupported[0] = VK_VERSION_MAJOR(vkPhysicalDeviceProperties.apiVersion);
        this->vulkanAPIVersionSupported[1] = VK_VERSION_MINOR(vkPhysicalDeviceProperties.apiVersion);
        this->vulkanAPIVersionSupported[2] = VK_VERSION_PATCH(vkPhysicalDeviceProperties.apiVersion);
        this->driverVersion = vkPhysicalDeviceProperties.driverVersion;
        this->deviceID = vkPhysicalDeviceProperties.deviceID;
    }
}