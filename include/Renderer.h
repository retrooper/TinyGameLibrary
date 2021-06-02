#pragma once

#include "Window.h"
#include "PipelineBuilder.h"
#include "GPU.h"
#include "VkBootstrap.h"
#include "Entity.h"
#include "DeletionQueue.h"
#include "AllocatedImage.h"
#include "Camera.h"
#include "Light.h"
#include "MeshRenderData.h"
#include <glm/gtx/transform.hpp>
#include <map>
#include <cstring>
#include <cmath>
#include <deque>
#define TGL_LOGGER_ENABLED
namespace tgl {
    struct FrameData {
        //Vulkan synchronization structures.
        //Wait semaphores will tell the GPU to wait for a certain semaphore to finish before executing its own task.
        //Signal semaphores will tell the GPU to unlock a certain semaphore once its execution is finished. We must reset semaphores before using them again.
        VkSemaphore vkPresentSemaphore{}, vkRenderSemaphore{};
        //With fences in Vulkan we can know from the CPU when the GPU has finished a task. We can wait for fences which blocks the CPU until GPU operation finishes.
        VkFence vkRenderFence{};

        VkCommandPool vkCommandPool;
        VkCommandBuffer vkMainCommandBuffer;

        AllocatedBuffer objectBuffer;
    };
    //Double buffering
    class Renderer {
    private:
        //Vulkan instance
        VkInstance vkInstance{};
        //Logical device
        VkDevice vkLogicalDevice{};

        //VMA
        VmaAllocator allocator{};

        //Window surface
        VkSurfaceKHR vkSurface{};

        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger{};

        //Swapchain
        VkSwapchainKHR vkSwapchain{};
        VkExtent2D vkWindowExtent{};
        VkFormat vkSwapchainImageFormat{};
        std::vector<VkImage> vkSwapchainImages{};
        std::vector<VkImageView> vkSwapchainImageViews{};
        //Queue
        VkQueue vkGraphicsQueue{};
        uint8_t vkGraphicsQueueFamilyIndex{};
        //Render pass
        //The renderpass allows us to tell the GPU that we are going to send some rendering commands allowing it to optimize. Subpasses also exist to allow it to optimize even further.
        VkRenderPass vkRenderPass{};
        //Framebuffers.The framebuffer links to the images you will render to, and it’s used when starting a renderpass to set the target images for rendering.
        std::vector<VkFramebuffer> vkFramebuffers{};

        uint32_t bufferingAmount;
        uint32_t frameCount;
        FrameData* frames;

        //Depth testing
        AllocatedImage depthImage{};
        VkImageView depthImageView{};

        std::vector<Entity> entities;

        void prepareVulkan();

        void initSwapchain();

        void initCommands();

        void initRenderpass();

        void initFramebuffers();

        void initSynchronizationStructures();

        void updateBuffers(Camera& camera, const Light& light);

        FrameData& getCurrentFrame();

    public:
        //Chosen GPU
        GPU gpu;
        Window *window;

        Renderer(Window *window, unsigned int bufferingAmount);
        ~Renderer();

        void init();

        void uploadEntity(Entity &entity);

        void registerEntity(Entity& entity);

        void registerEntities(std::vector<Entity>& entities);

        void clearEntities();

        void render(Camera& camera, Light& light);

        void destroy();
    };
}