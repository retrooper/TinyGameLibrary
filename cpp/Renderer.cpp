#include "Renderer.h"

namespace tgl {
    Renderer::Renderer(Window *window, unsigned int bufferingAmount) {
        this->window = window;
        this->bufferingAmount = bufferingAmount;
        this->frames = new FrameData[bufferingAmount];
    }

    Renderer::~Renderer() {
        delete[] frames;
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
            const char *glfwExtension = glfwExtensions[i];
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

            VK_HANDLE_ERROR(glfwCreateWindowSurface(vkInstance, window->glfwWindow, nullptr, &vkSurface),
                            "Failed to create a window surface!");

            VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures{};
            vkPhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
            vkPhysicalDeviceFeatures.sampleRateShading = VK_TRUE;
            vkb::PhysicalDeviceSelector physicalDeviceSelector(vkb_inst);
            auto phys_ret = physicalDeviceSelector
                    .set_surface(vkSurface)
                    .set_desired_version(1, 0)
                    .set_required_features(vkPhysicalDeviceFeatures)
                    .select();
            if (phys_ret.has_value()) {
                gpu = GPU(phys_ret.value().physical_device);
            } else {
                ERROR("Failed to find a supported GPU!");
            }

            //Create logical device
            vkb::DeviceBuilder vkbLogicalDeviceBuilder(phys_ret.value());
            vkb::Device vkbLogicalDevice = vkbLogicalDeviceBuilder
                    .build().value();
            vkLogicalDevice = vkbLogicalDevice.device;
            vkGraphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
            vkGraphicsQueueFamilyIndex = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
            VmaAllocatorCreateInfo vmaAllocatorCreateInfo{};
            vmaAllocatorCreateInfo.instance = vkInstance;
            vmaAllocatorCreateInfo.device = vkLogicalDevice;
            vmaAllocatorCreateInfo.physicalDevice = gpu.vkPhysicalDevice;
            vmaCreateAllocator(&vmaAllocatorCreateInfo, &allocator);
        } else {
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

            VkImageCreateInfo vkImageCreateInfo{};
            vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            vkImageCreateInfo.format = VK_FORMAT_D32_SFLOAT;
            vkImageCreateInfo.extent = {vkWindowExtent.width, vkWindowExtent.height, 1};
            vkImageCreateInfo.mipLevels = 1;
            vkImageCreateInfo.arrayLayers = 1;
            vkImageCreateInfo.samples = VkUtils::getMaxUsableSampleCount(gpu);
            vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            vkImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

            VmaAllocationCreateInfo depthImageAllocationCreateInfo{};
            depthImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            depthImageAllocationCreateInfo.flags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            VK_HANDLE_ERROR(
                    vmaCreateImage(allocator, &vkImageCreateInfo, &depthImageAllocationCreateInfo, &depthImage.image,
                                   &depthImage.allocation, nullptr),
                    "Failed to create the depth image!");

            VkUtils::createImageView(vkLogicalDevice, depthImage.image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT,
                                     &depthImageView);

            DeletionQueue::queue([=]() {
                vkDestroyImageView(vkLogicalDevice, depthImageView, nullptr);
                vmaDestroyImage(allocator, depthImage.image, depthImage.allocation);
            });
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
        for (uint32_t i = 0; i < bufferingAmount; i++) {
            VK_HANDLE_ERROR(
                    vkCreateCommandPool(vkLogicalDevice, &vkCommandPoolCreateInfo, nullptr, &frames[i].vkCommandPool),
                    "Failed to create a command pool!");

            VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
            vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            vkCommandBufferAllocateInfo.pNext = nullptr;

            //commands will be made from our _commandPool
            vkCommandBufferAllocateInfo.commandPool = frames[i].vkCommandPool;
            //we will allocate 1 command buffer
            vkCommandBufferAllocateInfo.commandBufferCount = 1;
            // command level is Primary
            vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


            VK_HANDLE_ERROR(
                    vkAllocateCommandBuffers(vkLogicalDevice, &vkCommandBufferAllocateInfo,
                                             &frames[i].vkMainCommandBuffer),
                    "Failed to allocate the main command buffer!");
            DeletionQueue::queue([=]() {
                vkDestroyCommandPool(vkLogicalDevice, frames[i].vkCommandPool, nullptr);
            });
        }
    }

    void Renderer::initRenderpass() {
        VkAttachmentDescription vkColorAttachmentDescription{};
        vkColorAttachmentDescription.format = vkSwapchainImageFormat;
        //We won't be doing MSAA
        vkColorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        //Clear when the attachment is loaded
        vkColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        vkColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //we don't care about stencil yet
        vkColorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        vkColorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //we don't know or care about the starting layout of the attachment
        vkColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //after the renderpass ends, the image has to be on a layout ready for display
        vkColorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference vkColorAttachmentRef{};
        vkColorAttachmentRef.attachment = 0;
        vkColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        VkAttachmentDescription vkDepthAttachmentDescription{};
        vkDepthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
        vkDepthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        vkDepthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        vkDepthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        vkDepthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        vkDepthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        vkDepthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkDepthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference vkDepthAttachmentRef{};
        vkDepthAttachmentRef.attachment = 1;
        vkDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &vkColorAttachmentRef;
        subpassDescription.pDepthStencilAttachment = &vkDepthAttachmentRef;

        std::vector<VkAttachmentDescription> attachments;
        attachments.push_back(vkColorAttachmentDescription);
        attachments.push_back(vkDepthAttachmentDescription);

        VkRenderPassCreateInfo vkRenderPassCreateInfo{};
        vkRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassCreateInfo.attachmentCount = attachments.size();
        vkRenderPassCreateInfo.pAttachments = attachments.data();
        vkRenderPassCreateInfo.subpassCount = 1;
        vkRenderPassCreateInfo.pSubpasses = &subpassDescription;
        VK_HANDLE_ERROR(vkCreateRenderPass(vkLogicalDevice, &vkRenderPassCreateInfo, nullptr, &vkRenderPass),
                        "Failed to create a renderpass!");
    }

    void Renderer::initFramebuffers() {
        VkFramebufferCreateInfo vkFramebufferCreateInfo{};
        //vkFramebufferCreateInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
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
            VkImageView attachments[2] = {vkSwapchainImageViews[i], depthImageView};
            vkFramebufferCreateInfo.pAttachments = attachments;
            vkFramebufferCreateInfo.attachmentCount = 2;
            VK_HANDLE_ERROR(vkCreateFramebuffer(vkLogicalDevice, &vkFramebufferCreateInfo, nullptr, &vkFramebuffers[i]),
                            "Failed to create the framebuffer at index " << i);
        }
    }

    void Renderer::initSynchronizationStructures() {
        VkFenceCreateInfo vkFenceCreateInfo{};
        vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        //we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
        vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


        VkSemaphoreCreateInfo vkSemaphoreCreateInfo{};
        vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkSemaphoreCreateInfo.pNext = nullptr;

        for (uint32_t i = 0; i < bufferingAmount; i++) {
            VK_HANDLE_ERROR(vkCreateFence(vkLogicalDevice, &vkFenceCreateInfo, nullptr, &frames[i].vkRenderFence),
                            "Failed to create the render fence!");
            DeletionQueue::queue([=]() {
                vkDestroyFence(vkLogicalDevice, frames[i].vkRenderFence, nullptr);
            });
            VK_HANDLE_ERROR(
                    vkCreateSemaphore(vkLogicalDevice, &vkSemaphoreCreateInfo, nullptr, &frames[i].vkPresentSemaphore),
                    "Failed to create a present semaphore!");
            VK_HANDLE_ERROR(
                    vkCreateSemaphore(vkLogicalDevice, &vkSemaphoreCreateInfo, nullptr, &frames[i].vkRenderSemaphore),
                    "Failed to create a render semaphore!");
            DeletionQueue::queue([=]() {
                vkDestroySemaphore(vkLogicalDevice, frames[i].vkPresentSemaphore, nullptr);
                vkDestroySemaphore(vkLogicalDevice, frames[i].vkRenderSemaphore, nullptr);
            });
        }
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
        vkGraphicsPipeline = pipelineBuilder.build(vkLogicalDevice, gpu, vkRenderPass,
                                                   vkVertexShaderModule, vkFragmentShaderModule,
                                                   vkViewport, vkScissor,
                                                   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                   VK_POLYGON_MODE_FILL,
                                                   VK_CULL_MODE_BACK_BIT,
                                                   VK_FRONT_FACE_CLOCKWISE, true, true);


        vkDestroyShaderModule(vkLogicalDevice, vkVertexShaderModule, nullptr);
        vkDestroyShaderModule(vkLogicalDevice, vkFragmentShaderModule, nullptr);
        DeletionQueue::queue([=]() {
            vkDestroyPipelineLayout(vkLogicalDevice, pipelineBuilder.vkPipelineLayout, nullptr);
            vkDestroyPipeline(vkLogicalDevice, vkGraphicsPipeline, nullptr);
        });

    }

    void Renderer::updateBuffers(Camera &camera, const Light &light) {
        glm::mat4 cameraTranslation = glm::translate(camera.position);
        glm::vec3 rotAxisX = {1, 0, 0};
        glm::vec3 rotAxisY = {0, 1, 0};
        glm::vec3 rotAxisZ = {0, 0, 1};
        glm::mat4 rotationX = glm::rotate(camera.pitch, rotAxisX);
        glm::mat4 rotationY = glm::rotate(camera.yaw, rotAxisY);
        glm::mat4 rotationZ = glm::rotate(camera.roll, rotAxisZ);
        //camera view matrix
        glm::mat4 camMatrix = cameraTranslation * rotationZ * rotationY * rotationX;
        camera.data.view = glm::inverse(camMatrix);
        camera.forward = camMatrix[2];
        camera.right = camMatrix[0];
        //Camera forward linear
        camMatrix = cameraTranslation * rotationY;
        camera.forwardLinear = camMatrix[2];
        //camera projection
        camera.data.projection = glm::perspectiveLH((camera.fov / 100.0F), window->aspect,
                                                    camera.nearClipPlane, camera.farClipPlane);
        for (auto &it : entityMap) {
            for (Entity &entity : it.second) {
                glm::mat4 translationMatrix = glm::translate(entity.position);
                glm::mat4 entityRotationX = glm::rotate(entity.pitch + M_PI_2f32, rotAxisX);
                glm::mat4 entityRotationY = glm::rotate(entity.yaw, rotAxisY);
                glm::mat4 entityRotationZ = glm::rotate(entity.roll, rotAxisZ);
                glm::mat4 rotationMatrix = entityRotationX * entityRotationY * entityRotationZ;
                glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), entity.scale);
                entity.mesh.data.model = translationMatrix * rotationMatrix * scaleMatrix;
                entity.mesh.data.lightPos = light.position;
            }
        }
    }

    FrameData &Renderer::getCurrentFrame() {
        return frames[frameCount % bufferingAmount];
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

    void Renderer::uploadMesh(Mesh &mesh) {
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = mesh.description.vertices.size() * sizeof(Vertex);
        vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        //Allocated by the CPU, visible/readable by the GPU.
        vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        VK_HANDLE_ERROR(
                vmaCreateBuffer(allocator, &vkBufferCreateInfo,
                                &vmaAllocationCreateInfo,
                                &mesh.description.vertexBuffer.vkBuffer, &mesh.description.vertexBuffer.allocation,
                                nullptr),
                "Failed to create a vertex buffer for a mesh!");

        vkBufferCreateInfo.size = mesh.description.indices.size() * sizeof(uint32_t);
        vkBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        VK_HANDLE_ERROR(
                vmaCreateBuffer(allocator, &vkBufferCreateInfo,
                                &vmaAllocationCreateInfo,
                                &mesh.description.indexBuffer.vkBuffer, &mesh.description.indexBuffer.allocation,
                                nullptr),
                "Failed to create an index for a mesh!");

        vkBufferCreateInfo.size = mesh.description.indices.size() * sizeof(MeshRenderData);
        vkBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VK_HANDLE_ERROR(
                vmaCreateBuffer(allocator, &vkBufferCreateInfo,
                                &vmaAllocationCreateInfo,
                                &mesh.meshDataBuffer.vkBuffer, &mesh.meshDataBuffer.allocation, nullptr),
                "Failed to create a mesh data buffer!"
        )

        DeletionQueue::queue([=]() {
            vmaDestroyBuffer(allocator, mesh.description.vertexBuffer.vkBuffer,
                             mesh.description.vertexBuffer.allocation);
            vmaDestroyBuffer(allocator, mesh.description.indexBuffer.vkBuffer, mesh.description.indexBuffer.allocation);
            vmaDestroyBuffer(allocator, mesh.meshDataBuffer.vkBuffer, mesh.meshDataBuffer.allocation);
        });

        //Converted our vertices data into GPU readable data
        void *data;
        vmaMapMemory(allocator, mesh.description.vertexBuffer.allocation, &data);
        memcpy(data, mesh.description.vertices.data(), mesh.description.vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(allocator, mesh.description.vertexBuffer.allocation);

        vmaMapMemory(allocator, mesh.description.indexBuffer.allocation, &data);
        memcpy(data, mesh.description.indices.data(), mesh.description.indices.size() * sizeof(uint32_t));
        vmaUnmapMemory(allocator, mesh.description.indexBuffer.allocation);

        VkDescriptorBufferInfo vkDescriptorBufferInfo;
        vkDescriptorBufferInfo.buffer = mesh.meshDataBuffer.vkBuffer;
        vkDescriptorBufferInfo.offset = 0; //Whole buffer
        vkDescriptorBufferInfo.range = sizeof(mesh.data);

        VkWriteDescriptorSet vkWriteDescriptorSet;
        vkWriteDescriptorSet.pNext = nullptr;
        vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vkWriteDescriptorSet.dstSet = pipelineBuilder.vkDescriptorSet;
        vkWriteDescriptorSet.dstBinding = 0;//Binding to update
        vkWriteDescriptorSet.dstArrayElement = 0;
        vkWriteDescriptorSet.descriptorCount = 1;
        vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vkWriteDescriptorSet.pImageInfo = nullptr;
        vkWriteDescriptorSet.pBufferInfo = &vkDescriptorBufferInfo;
        vkWriteDescriptorSet.pTexelBufferView = nullptr;
        std::vector<VkWriteDescriptorSet> vkWriteDescriptorSets;
        vkWriteDescriptorSets.reserve(1);
        vkWriteDescriptorSets.push_back(vkWriteDescriptorSet);

        vkUpdateDescriptorSets(vkLogicalDevice, vkWriteDescriptorSets.size(), vkWriteDescriptorSets.data(), 0,
                               nullptr);
        mesh.uploaded = true;
    }

    void Renderer::registerEntity(Entity &entity) {
        if (entityMap.find(entity.mesh.description) != entityMap.end()) {
            entityMap[entity.mesh.description].push_back(entity);
        } else {
            std::deque<Entity> newEntityQueue;
            newEntityQueue.push_back(entity);
            entityMap[entity.mesh.description] = newEntityQueue;
        }
    }

    void Renderer::registerEntities(std::vector<Entity> &entities) {
        for (Entity entity : entities) {
            registerEntity(entity);
        }
    }

    void Renderer::clearEntities() {
        entityMap.clear();
    }

    void Renderer::render(Camera &camera, Light &light) {
        FrameData frameData = getCurrentFrame();

        //wait until the GPU has finished rendering the last frame.
        uint32_t vkSwapchainImageIndex;
        VK_HANDLE_ERROR(
                vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, 1000000000, frameData.vkPresentSemaphore, nullptr,
                                      &vkSwapchainImageIndex), "Failed to acquire the next image!");
        VK_HANDLE_ERROR(vkWaitForFences(vkLogicalDevice, 1, &frameData.vkRenderFence, true, 1000000000),
                        "Failed to wait for render fence!");
        VK_HANDLE_ERROR(vkResetFences(vkLogicalDevice, 1, &frameData.vkRenderFence),
                        "Failed to reset the render fence!");

        /**
         * UPDATE BUFFERS
         */
        updateBuffers(camera, light);

        VK_HANDLE_ERROR(vkResetCommandBuffer(frameData.vkMainCommandBuffer, 0),
                        "Failed to reset the main command buffer!");

        VkUtils::beginCommandBuffer(frameData.vkCommandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                    &frameData.vkMainCommandBuffer);
        //background color
        VkClearValue vkClearValueDefault{};
        VkClearValue vkClearValues[2] = {vkClearValueDefault, vkClearValueDefault};
        vkClearValues[0].color = {{window->backgroundColor.r, window->backgroundColor.g,
                                          window->backgroundColor.b, window->backgroundColor.a}};
        vkClearValues[1].depthStencil.depth = 1.0f;

        VkRenderPassBeginInfo vkRenderPassBeginInfo{};
        vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        vkRenderPassBeginInfo.renderPass = vkRenderPass;
        vkRenderPassBeginInfo.framebuffer = vkFramebuffers[vkSwapchainImageIndex];
        vkRenderPassBeginInfo.clearValueCount = 2;
        vkRenderPassBeginInfo.pClearValues = vkClearValues;
        vkRenderPassBeginInfo.renderArea.offset.x = 0;
        vkRenderPassBeginInfo.renderArea.offset.y = 0;
        vkRenderPassBeginInfo.renderArea.extent = vkWindowExtent;

        //We don't care about the image layout yet
        vkCmdBeginRenderPass(frameData.vkMainCommandBuffer, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


        vkCmdBindPipeline(frameData.vkMainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);
        std::vector<CameraData> dataList;
        dataList.push_back(camera.data);
        dataList.reserve(1);

        vkCmdPushConstants(frameData.vkMainCommandBuffer, pipelineBuilder.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                           0,
                           sizeof(CameraData), dataList.data());

        for (auto &it : entityMap) {
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(frameData.vkMainCommandBuffer, 0, 1, &it.first.vertexBuffer.vkBuffer,
                                   &offset);
            vkCmdBindIndexBuffer(frameData.vkMainCommandBuffer, it.first.indexBuffer.vkBuffer, offset,
                                 VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(frameData.vkMainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineBuilder.vkPipelineLayout,
                                    0, 1,
                                    &pipelineBuilder.vkDescriptorSet, 0, nullptr);
            for (Entity &entity : it.second) {
                void *data;
                vmaMapMemory(allocator, entity.mesh.meshDataBuffer.allocation, &data);
                memcpy(data, &entity.mesh.data, sizeof(MeshRenderData));
                vmaUnmapMemory(allocator, entity.mesh.meshDataBuffer.allocation);

                uint32_t indexSize = it.first.indices.size();
                //we can now draw the entity
                vkCmdDrawIndexed(frameData.vkMainCommandBuffer, indexSize, 1, 0, 0, 0);
            }
        }
        //The render pass transitions the image into the format ready for display.
        vkCmdEndRenderPass(frameData.vkMainCommandBuffer);
        vkEndCommandBuffer(frameData.vkMainCommandBuffer);

        //We can submit the command buffer to the GPU
        //prepare the submission to the queue.
        //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
        //we will signal the _renderSemaphore, to signal that rendering has finished
        VkSubmitInfo vkSubmitInfo{};
        vkSubmitInfo.pNext = nullptr;
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.pCommandBuffers = &frameData.vkMainCommandBuffer;
        vkSubmitInfo.waitSemaphoreCount = 1;
        vkSubmitInfo.pWaitSemaphores = &frameData.vkPresentSemaphore;
        vkSubmitInfo.signalSemaphoreCount = 1;
        vkSubmitInfo.pSignalSemaphores = &frameData.vkRenderSemaphore;
        VkPipelineStageFlags vkPipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSubmitInfo.pWaitDstStageMask = &vkPipelineStageFlags;

        //submit command buffer to the queue and execute it.
        // _renderFence will now block until the graphic commands finish execution
        VK_HANDLE_ERROR(vkQueueSubmit(vkGraphicsQueue, 1, &vkSubmitInfo, frameData.vkRenderFence),
                        "Failed to submit a command buffer to the queue for execution!");

        //Now display the image to the screen
        VkPresentInfoKHR vkPresentInfo{};
        vkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        vkPresentInfo.waitSemaphoreCount = 1;
        //Wait for the rendering to finish before we present
        vkPresentInfo.pWaitSemaphores = &frameData.vkRenderSemaphore;
        vkPresentInfo.swapchainCount = 1;
        vkPresentInfo.pSwapchains = &vkSwapchain;
        vkPresentInfo.pImageIndices = &vkSwapchainImageIndex;

        VK_HANDLE_ERROR(vkQueuePresentKHR(vkGraphicsQueue, &vkPresentInfo),
                        "Failed to present an image to the screen!");
        frameCount++;
    }

    void Renderer::destroy() {
        vkQueueWaitIdle(vkGraphicsQueue);
        DeletionQueue::flush();
        vkDestroyDescriptorSetLayout(vkLogicalDevice, pipelineBuilder.vkDescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(vkLogicalDevice, pipelineBuilder.vkDescriptorPool, nullptr);
        vkDestroySwapchainKHR(vkLogicalDevice, vkSwapchain, nullptr);
        vkDestroyRenderPass(vkLogicalDevice, vkRenderPass, nullptr);
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