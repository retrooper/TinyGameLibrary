#pragma once
#include "Window.h"
#include "PipelineBuilder.h"
#include "GPU.h"
#include "VkBootstrap.h"
#include "Mesh.h"
#include "DeletionQueue.h"
#include <glm/gtx/transform.hpp>
#include <cstring>
#define TGL_LOGGER_ENABLED
namespace tgl {
    class Renderer {
    private:
        //Vulkan instance
        VkInstance vkInstance{};
        //Logical device
        VkDevice vkLogicalDevice{};

        //VMA
        VmaAllocator allocator;

        //Window surface
        VkSurfaceKHR vkSurface{};

        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger;

        //Swapchain
        VkSwapchainKHR vkSwapchain{};
        VkExtent2D vkWindowExtent{};
        VkFormat vkSwapchainImageFormat{};
        std::vector<VkImage> vkSwapchainImages{};
        std::vector<VkImageView> vkSwapchainImageViews{};
        //Queue
        VkQueue vkGraphicsQueue{};
        uint8_t vkGraphicsQueueFamilyIndex{};
        //Command pool & buffers. We send commands to the GPU through command buffers to the queue. The queue will then make sure they get executed.
        VkCommandPool vkCommandPool{}; //Only one pool per thread is allowed. Submitting a command buffer can not be done in from two threads at the same time.
        VkCommandBuffer vkMainCommandBuffer{}; //Main command buffer, we will use only one for now.
        //Render pass
        //The renderpass allows us to tell the GPU that we are going to send some rendering commands allowing it to optimize. Subpasses also exist to allow it to optimize even further.
        VkRenderPass vkRenderPass{};
        //Framebuffers.The framebuffer links to the images you will render to, and it’s used when starting a renderpass to set the target images for rendering.
        std::vector<VkFramebuffer> vkFramebuffers{};

        //Vulkan synchronization structures.
        //Wait semaphores will tell the GPU to wait for a certain semaphore to finish before executing its own task.
        //Signal semaphores will tell the GPU to unlock a certain semaphore once its execution is finished. We must reset semaphores before using them again.
        VkSemaphore vkPresentSemaphore{}, vkRenderSemaphore{};
        //With fences in Vulkan we can know from the CPU when the GPU has finished a task. We can wait for fences which blocks the CPU until GPU operation finishes.
        VkFence vkRenderFence{};

        //Shaders
        VkShaderModule vkVertexShaderModule{};
        VkShaderModule vkFragmentShaderModule{};

        PipelineBuilder pipelineBuilder;
        VkPipeline vkGraphicsPipeline{};

        void prepareVulkan();
        void initSwapchain();
        void initCommands();
        void initRenderpass();
        void initFramebuffers();
        void initSynchronizationStructures();
        void initShaders();
        void initGraphicsPipeline();
    public:
        //Chosen GPU
        GPU gpu;
        Window* window;
        explicit Renderer(Window* window);

        void init();

        void uploadMesh(Mesh& mesh);

        void render(Mesh& mesh);

        void destroy();
    };
}