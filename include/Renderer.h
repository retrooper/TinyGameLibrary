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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
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

        unsigned int bufferingAmount;
        FrameData* frames;

        //Shaders
        VkShaderModule vkVertexShaderModule{};
        VkShaderModule vkFragmentShaderModule{};

        PipelineBuilder pipelineBuilder;
        VkPipeline vkGraphicsPipeline{};

        //Depth testing
        AllocatedImage depthImage{};
        VkImageView depthImageView{};

        std::map<MeshDescription, std::deque<Entity>> entityMap;

        unsigned int frameCount = 0;

        ImGuiIO imGuiIO;

        void prepareVulkan();

        void initSwapchain();

        void initCommands();

        void initRenderpass();

        void initFramebuffers();

        void initSynchronizationStructures();

        void initShaders();

        void initGraphicsPipeline();

        void initImGui();

        FrameData& getCurrentFrame();


        void uploadMesh(Mesh &mesh);

    public:
        //Chosen GPU
        GPU gpu;
        Window *window;
        glm::vec3 forward = {0, 1, 0};
        glm::vec3 back = -forward;
        glm::vec3 left = {-1, 0, 0};
        glm::vec3 right = -left;

        Renderer(Window *window, unsigned int bufferingAmount);
        ~Renderer();

        void init();

        void registerEntity(Entity& entity);

        void registerEntities(std::vector<Entity>& entities);

        void render(Camera& camera, Light& light);

        void destroy();
    };
}