#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#ifdef TGL_LOGGER_ENABLED
#define INFO(msg) std::cout << "TGL INFO: " << msg << std::endl
#define WARN(msg) std::cout << "TGL WARN: " << msg << std::endl
#define ERROR_NO_EXIT(msg) std::cerr << "TGL ERROR: " << msg << std::endl
#define ERROR(msg) std::cerr << "TGL ERROR: " << msg << std::endl; exit(-1)
#else
#define INFO(msg)
#define WARN(msg)
#define ERROR(msg)
#define ERROR_NO_EXIT(msg)
#endif

#define VK_HANDLE_ERROR_NO_EXIT(vkResult, ERROR_MSG) if (vkResult != VK_SUCCESS) { ERROR_NO_EXIT(ERROR_MSG << " Error code: " << vkResult); }
#define VK_HANDLE_ERROR(vkResult, ERROR_MSG) if (vkResult != VK_SUCCESS) { ERROR(ERROR_MSG << " Error code: " << vkResult); }
#define VK_HANDLE_SUCCESS_AND_ERROR(vkResult, SUCCESS_MSG, ERROR_MSG) if (vkResult == VK_SUCCESS) {  INFO(SUCCESS_MSG); } else {  ERROR(ERROR_MSG << " Error code: " << vkResult); }
#define VK_HANDLE_SUCCESS_AND_ERROR_NO_EXIT(vkResult, SUCCESS_MSG, ERROR_MSG) if (result == VK_SUCCESS) {  INFO(SUCCESS_MSG); } else {  ERROR_NO_EXIT(ERROR_MSG << "Error code: " << vkResult); }

#include <vulkan/vulkan.h>
#include "GPU.h"
namespace tgl {
    class VkUtils {
    public:
        static void beginCommandBuffer(VkCommandPool& vkCommandPool, VkCommandBufferUsageFlags vkCommandBufferUsageFlags, VkCommandBuffer* vkCommandBuffer);
        static std::vector<uint32_t> readFile(const std::string& fileName);
        static VkShaderModule createShaderModule(VkDevice& vkLogicalDevice, std::vector<uint32_t> &shaderCode);
        static int getOptimalSwapchainImageCount(GPU& gpu);
        static VkFormat getOptimalSwapchainFormat();
        static VkPresentModeKHR getOptimalPresentMode(std::vector<VkPresentModeKHR>& vkPresentModes);
        static void createImageView(VkDevice &vkLogicalDevice, VkImage &vkImage, VkFormat vkFormat,
                                           VkImageAspectFlags vkImageAspectFlags, VkImageView *vkImageView);
        static void submitCommandBufferImmediately(VkDevice& vkLogicalDevice, VkQueue& vkQueue, VkCommandPool& vkCommandPool, std::function<void(VkCommandBuffer& vkCommandBuffer)> task);
        static VkSampleCountFlagBits getMaxUsableSampleCount(GPU& gpu);
    };
}