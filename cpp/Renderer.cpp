#include "Renderer.h"

namespace tgl {
    Renderer::Renderer(Window *window) {
        this->window = window;
    }

    void Renderer::prepareVulkan() {
        if (!window->hasCreated()) {
            window->create();
        }

        uint32_t glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        vkb::InstanceBuilder builder;

        //make the Vulkan instance, with basic debug features
        builder = builder.set_app_name("TGL Application")
                .request_validation_layers(true)
                .require_api_version(1, 0, 0)
                .use_default_debug_messenger();
        std::cout << "COUNT: " << glfwExtensionCount << std::endl;
        for (int i = 0; i < glfwExtensionCount; i++) {
            const char* glfwExtension = glfwExtensions[i];
            std::cout << "Extension: " << glfwExtension << std::endl;
            builder = builder.enable_extension(glfwExtension);
        }
        auto inst_ret = builder.build();
        if (inst_ret.has_value()) {
            vkb::Instance vkb_inst = inst_ret.value();

            //store the instance
            vkInstance = vkb_inst.instance;
            //store the debug messenger
            vkDebugUtilsMessenger = vkb_inst.debug_messenger;

            VK_HANDLE_ERROR(glfwCreateWindowSurface(vkInstance, window->glfwWindow, nullptr, &vkSurface), "Failed to create a window surface!");

            vkb::PhysicalDeviceSelector physicalDeviceSelector(vkb_inst);
            auto phys_ret = physicalDeviceSelector
                    .set_surface(vkSurface)
                    .set_desired_version(1, 0)
                    .select();
            if (phys_ret.has_value()) {
                gpu = GPU(phys_ret.value().physical_device);
            }
            else {
                ERROR("Failed to find a supported GPU!");
            }

            //Create logical device
            vkb::DeviceBuilder vkbLogicalDeviceBuilder(phys_ret.value());
            vkb::Device vkbLogicalDevice = vkbLogicalDeviceBuilder.build().value();
            vkLogicalDevice = vkbLogicalDevice.device;
            vkGraphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
            vkGraphicsQueueFamilyIndex = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();

            VmaAllocatorCreateInfo vmaAllocatorCreateInfo{};
            vmaAllocatorCreateInfo.instance = vkInstance;
            vmaAllocatorCreateInfo.device = vkLogicalDevice;
            vmaAllocatorCreateInfo.physicalDevice = gpu.vkPhysicalDevice;
            vmaCreateAllocator(&vmaAllocatorCreateInfo, &allocator);
        }
        else {
            ERROR("Failed to create a vulkan instance!");
        }
    }

    void Renderer::initSwapchain() {
        vkb::SwapchainBuilder vkbSwapchainBuilder{gpu.vkPhysicalDevice, vkLogicalDevice, vkSurface};
        auto vkbSwapchainOpt = vkbSwapchainBuilder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(window->width, window->height)
                .build();
        if (vkbSwapchainOpt.has_value()) {
            auto vkbSwapchain = vkbSwapchainOpt.value();
            vkSwapchain = vkbSwapchain.swapchain;
            vkSwapchainImages = vkbSwapchain.get_images().value();
            vkSwapchainImageViews = vkbSwapchain.get_image_views().value();
            vkSwapchainImageFormat = vkbSwapchain.image_format;
            vkWindowExtent = vkbSwapchain.extent;
        } else {
            ERROR("Failed to create a swapchain! Error: " << vkbSwapchainOpt.error());
        }
    }

    void Renderer::initCommands() {
        VkCommandPoolCreateInfo vkCommandPoolCreateInfo{};
        vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        vkCommandPoolCreateInfo.pNext = nullptr;
        //We want to allow the resetting of individual command buffers
        vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCommandPoolCreateInfo.queueFamilyIndex = vkGraphicsQueueFamilyIndex;
        VK_HANDLE_ERROR(vkCreateCommandPool(vkLogicalDevice, &vkCommandPoolCreateInfo, nullptr, &vkCommandPool),
                        "Failed to create a command pool!");

        VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
        vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        vkCommandBufferAllocateInfo.pNext = nullptr;

        //commands will be made from our _commandPool
        vkCommandBufferAllocateInfo.commandPool = vkCommandPool;
        //we will allocate 1 command buffer
        vkCommandBufferAllocateInfo.commandBufferCount = 1;
        // command level is Primary
        vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


        VK_HANDLE_ERROR(vkAllocateCommandBuffers(vkLogicalDevice, &vkCommandBufferAllocateInfo, &vkMainCommandBuffer),
                        "Failed to allocate the main command buffer!");
    }

    void Renderer::initRenderpass() {
        VkAttachmentDescription vkAttachmentDescription{};
        vkAttachmentDescription.format = vkSwapchainImageFormat;
        //We won't be doing MSAA
        vkAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        //Clear when the attachment is loaded
        vkAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        vkAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //we don't care about stencil yet
        vkAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //we don't know or care about the starting layout of the attachment
        vkAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //after the renderpass ends, the image has to be on a layout ready for display
        vkAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference vkAttachmentReference{};
        vkAttachmentReference.attachment = 0;
        vkAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription vkSubpassDescription{};
        vkSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        vkSubpassDescription.colorAttachmentCount = 1;
        vkSubpassDescription.pColorAttachments = &vkAttachmentReference;

        VkRenderPassCreateInfo vkRenderPassCreateInfo{};
        vkRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassCreateInfo.attachmentCount = 1;
        vkRenderPassCreateInfo.pAttachments = &vkAttachmentDescription;
        vkRenderPassCreateInfo.subpassCount = 1;
        vkRenderPassCreateInfo.pSubpasses = &vkSubpassDescription;
        VK_HANDLE_ERROR(vkCreateRenderPass(vkLogicalDevice, &vkRenderPassCreateInfo, nullptr, &vkRenderPass),
                        "Failed to create a renderpass!");
    }

    void Renderer::initFramebuffers() {
        VkFramebufferCreateInfo vkFramebufferCreateInfo{};
        vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        vkFramebufferCreateInfo.renderPass = vkRenderPass;
        vkFramebufferCreateInfo.attachmentCount = 1;
        vkFramebufferCreateInfo.pAttachments = vkSwapchainImageViews.data();
        vkFramebufferCreateInfo.width = vkWindowExtent.width;
        vkFramebufferCreateInfo.height = vkWindowExtent.height;
        vkFramebufferCreateInfo.layers = 1;
        //create a framebuffer for each of the swapchain image view
        vkFramebuffers.resize(vkSwapchainImageViews.size());
        for (int i = 0; i < vkSwapchainImageViews.size(); i++) {
            vkFramebufferCreateInfo.pAttachments = &vkSwapchainImageViews[i];
            VK_HANDLE_ERROR(vkCreateFramebuffer(vkLogicalDevice, &vkFramebufferCreateInfo, nullptr, &vkFramebuffers[i]),
                            "Failed to create the framebuffer at index " << i);
        }
    }

    void Renderer::initSynchronizationStructures() {
        VkFenceCreateInfo vkFenceCreateInfo{};
        vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        //we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
        vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_HANDLE_ERROR(vkCreateFence(vkLogicalDevice, &vkFenceCreateInfo, nullptr, &vkRenderFence),
                        "Failed to create the render fence!");

        VkSemaphoreCreateInfo vkSemaphoreCreateInfo{};
        vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkSemaphoreCreateInfo.pNext = nullptr;
        VK_HANDLE_ERROR(vkCreateSemaphore(vkLogicalDevice, &vkSemaphoreCreateInfo, nullptr, &vkPresentSemaphore),
                        "Failed to create the present semaphore!");
        VK_HANDLE_ERROR(vkCreateSemaphore(vkLogicalDevice, &vkSemaphoreCreateInfo, nullptr, &vkRenderSemaphore),
                        "Failed to create the render semaphore!");
    }

    void Renderer::initShaders() {
        std::vector<uint32_t> vertexShaderCode = VkUtils::readFile("../resources/shaders/vert.spv");
        vkVertexShaderModule = VkUtils::createShaderModule(vkLogicalDevice, vertexShaderCode);

        std::vector<uint32_t> fragmentShaderCode = VkUtils::readFile("../resources/shaders/frag.spv");
        vkFragmentShaderModule = VkUtils::createShaderModule(vkLogicalDevice, fragmentShaderCode);
    }

    void Renderer::initGraphicsPipeline() {
        VkViewport vkViewport{};
        vkViewport.x = 0;
        vkViewport.y = 0;
        vkViewport.width = vkWindowExtent.width;
        vkViewport.height = vkWindowExtent.height;
        vkViewport.minDepth = 0.0F;
        vkViewport.maxDepth = 1.0F;
        VkRect2D vkScissor{};
        vkScissor.extent = vkWindowExtent;
        vkScissor.offset.x = 0;
        vkScissor.offset.y = 0;
        vkGraphicsPipeline = pipelineBuilder.build(vkLogicalDevice, vkRenderPass,
                                                   vkVertexShaderModule, vkFragmentShaderModule,
                                                   vkViewport, vkScissor,
                                                   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                   VK_POLYGON_MODE_FILL,
                                                   VK_CULL_MODE_NONE,
                                                   VK_FRONT_FACE_CLOCKWISE);

        vkDestroyShaderModule(vkLogicalDevice, vkVertexShaderModule, nullptr);
        vkDestroyShaderModule(vkLogicalDevice, vkFragmentShaderModule, nullptr);
        DeletionQueue::queue([=](){
            vkDestroyPipelineLayout(vkLogicalDevice, pipelineBuilder.vkPipelineLayout, nullptr);
           vkDestroyPipeline(vkLogicalDevice, vkGraphicsPipeline, nullptr);
        });

    }

    void Renderer::init() {
        prepareVulkan();
        initSwapchain();
        initCommands();
        initRenderpass();
        initFramebuffers();
        initSynchronizationStructures();
        initShaders();
        initGraphicsPipeline();
    }

    void Renderer::uploadMesh(Mesh& mesh) {
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = mesh.vertices.size() * sizeof(Vertex);
        vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        //Allocated by the CPU, visible/readable by the GPU.
        vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        VK_HANDLE_ERROR(
                vmaCreateBuffer(allocator, &vkBufferCreateInfo,
                                &vmaAllocationCreateInfo,
                                &mesh.allocatedBuffer.vkBuffer,&mesh.allocatedBuffer.allocation, nullptr),
                                "Failed to create a buffer for a mesh!");

        DeletionQueue::queue([=]() {
            vmaDestroyBuffer(allocator, mesh.allocatedBuffer.vkBuffer, mesh.allocatedBuffer.allocation);
        });

        //Converted our vertices data into GPU readable data
        void* data;
        vmaMapMemory(allocator, mesh.allocatedBuffer.allocation, &data);
        memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));

        vmaUnmapMemory(allocator, mesh.allocatedBuffer.allocation);
    }

    void Renderer::render(Mesh& mesh) {
        int waitTimeout = 1000000000; //One second
        //wait until the GPU has finished rendering the last frame.
        VK_HANDLE_ERROR(vkWaitForFences(vkLogicalDevice, 1, &vkRenderFence, true, waitTimeout),
                        "Failed to wait for render fence!");
        VK_HANDLE_ERROR(vkResetFences(vkLogicalDevice, 1, &vkRenderFence), "Failed to reset the render fence!");

        uint32_t vkSwapchainImageIndex;
        VK_HANDLE_ERROR(vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, waitTimeout, vkPresentSemaphore, nullptr,
                                              &vkSwapchainImageIndex), "Failed to acquire the next image!");
        VK_HANDLE_ERROR(vkResetCommandBuffer(vkMainCommandBuffer, 0), "Failed to reset the main command buffer!");
        VkUtils::beginCommandBuffer(vkCommandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, &vkMainCommandBuffer);

        //background color
        float r = 0;
        float g = 0;
        float b = 1;
        float a = 1;
        VkClearValue vkClearValue{};
        vkClearValue.color = {{r, g, b, a}};
        VkRenderPassBeginInfo vkRenderPassBeginInfo{};
        vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        vkRenderPassBeginInfo.renderPass = vkRenderPass;
        vkRenderPassBeginInfo.framebuffer = vkFramebuffers[vkSwapchainImageIndex];
        vkRenderPassBeginInfo.clearValueCount = 1;
        vkRenderPassBeginInfo.pClearValues = &vkClearValue;
        vkRenderPassBeginInfo.renderArea.offset.x = 0;
        vkRenderPassBeginInfo.renderArea.offset.y = 0;
        vkRenderPassBeginInfo.renderArea.extent = vkWindowExtent;
        //We don't care about the image layout yet
        vkCmdBeginRenderPass(vkMainCommandBuffer, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(vkMainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);

        //bind the mesh vertex buffer with offset 0
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(vkMainCommandBuffer, 0, 1, &mesh.allocatedBuffer.vkBuffer, &offset);

        //we can now draw the mesh
        vkCmdDraw(vkMainCommandBuffer, mesh.vertices.size(), 1, 0, 0);

        //The render pass transitions the image into the format ready for display.
        vkCmdEndRenderPass(vkMainCommandBuffer);
        vkEndCommandBuffer(vkMainCommandBuffer);

        //We can submit the command buffer to the GPU
        //prepare the submission to the queue.
        //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
        //we will signal the _renderSemaphore, to signal that rendering has finished
        VkSubmitInfo vkSubmitInfo{};
        vkSubmitInfo.pNext = nullptr;
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.pCommandBuffers = &vkMainCommandBuffer;
        vkSubmitInfo.waitSemaphoreCount = 1;
        vkSubmitInfo.pWaitSemaphores = &vkPresentSemaphore;
        vkSubmitInfo.signalSemaphoreCount = 1;
        vkSubmitInfo.pSignalSemaphores = &vkRenderSemaphore;
        VkPipelineStageFlags vkPipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSubmitInfo.pWaitDstStageMask = &vkPipelineStageFlags;

        //submit command buffer to the queue and execute it.
        // _renderFence will now block until the graphic commands finish execution
        VK_HANDLE_ERROR(vkQueueSubmit(vkGraphicsQueue, 1, &vkSubmitInfo, vkRenderFence),
                        "Failed to submit a command buffer to the queue for execution!");

        //Now display the image to the screen
        VkPresentInfoKHR vkPresentInfo{};
        vkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        vkPresentInfo.waitSemaphoreCount = 1;
        //Wait for the rendering to finish before we present
        vkPresentInfo.pWaitSemaphores = &vkRenderSemaphore;
        vkPresentInfo.swapchainCount = 1;
        vkPresentInfo.pSwapchains = &vkSwapchain;
        vkPresentInfo.pImageIndices = &vkSwapchainImageIndex;

        VK_HANDLE_ERROR(vkQueuePresentKHR(vkGraphicsQueue, &vkPresentInfo),
                        "Failed to present an image to the screen!");
    }

    void Renderer::destroy() {
        vkQueueWaitIdle(vkGraphicsQueue);
        DeletionQueue::flush();
        vkDestroySemaphore(vkLogicalDevice, vkRenderSemaphore, nullptr);
        vkDestroySemaphore(vkLogicalDevice, vkPresentSemaphore, nullptr);
        vkDestroyFence(vkLogicalDevice, vkRenderFence, nullptr);
        vkDestroySwapchainKHR(vkLogicalDevice, vkSwapchain, nullptr);
        vkDestroyRenderPass(vkLogicalDevice, vkRenderPass, nullptr);
        vkDestroyCommandPool(vkLogicalDevice, vkCommandPool, nullptr);
        for (int i = 0; i < vkSwapchainImageViews.size(); i++) {
            vkDestroyFramebuffer(vkLogicalDevice, vkFramebuffers[i], nullptr);
            vkDestroyImageView(vkLogicalDevice, vkSwapchainImageViews[i], nullptr);
        }
        vmaDestroyAllocator(allocator);//TODO civ place
        vkDestroyDevice(vkLogicalDevice, nullptr);
        vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
        vkb::destroy_debug_utils_messenger(vkInstance, vkDebugUtilsMessenger, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
    }
}