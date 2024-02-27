#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ze_app.hpp"

#include <stdexcept>
#include <array>

namespace ze {

    struct SimplePushConstantData {
        glm::mat2 transform { 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    ZeApp::ZeApp() {
        loadGameObjects();
        createPipelineLayout();
        recreateSwapChain();
        createCommandsBuffers();
    }

    ZeApp::~ZeApp() {
        vkDestroyPipelineLayout(zeDevice.device(), pipelineLayout, nullptr);

    }

    void ZeApp::run() {
        while (!zeWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(zeDevice.device());
    }

    void ZeApp::loadGameObjects() {
        std::vector<ZeModel::Vertex> vertices {
                {{0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f }},
                {{0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f }},
                {{-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f }}
        };
        auto zeModel = std::make_shared<ZeModel>(zeDevice, vertices);
        auto triangle = ZeGameObject::createGameObject();
        triangle.model = zeModel;
        triangle.color = { 0.1f, 0.8f, 0.1f };
        triangle.transform2D.translation.x = 0.2f;
        triangle.transform2D.scale = { 2.0f, 0.5 };
        triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void ZeApp::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(zeDevice.device(), &pipelineLayoutCreateInfo, nullptr,&pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void ZeApp::createPipeline() {
        assert(zeSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before layout");

        PipelineConfigInfo pipelineConfigInfo{};
        ZePipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
        pipelineConfigInfo.renderPass = zeSwapChain->getRenderPass();
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        zePipeline = std::make_unique<ZePipeline>(
                zeDevice,
                "shaders/simple_shader.vert.spv",
                "shaders/simple_shader.frag.spv",
                pipelineConfigInfo
                );
    }

    void ZeApp::createCommandsBuffers() {
        commandBuffers.resize(zeSwapChain->imageCount());
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = zeDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(zeDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void ZeApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = zeSwapChain->getRenderPass();
        renderPassBeginInfo.framebuffer = zeSwapChain->getFrameBuffer(imageIndex);

        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = zeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = { 1.0f,  0 };
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float >(zeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float >(zeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, zeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport (commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffer");

        }
    }


    void ZeApp::renderGameObjects(VkCommandBuffer commandBuffer) {
        zePipeline->bind(commandBuffer);
        for (auto& obj: gameObjects) {
            obj.transform2D.rotation = glm::mod(obj.transform2D.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2D.translation;
            push.color = obj.color;
            push.transform = obj.transform2D.mat2();

            vkCmdPushConstants(commandBuffer,
                               pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0,
                               sizeof(SimplePushConstantData),
                               &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

    void ZeApp::freeCommanBuffers() {
        vkFreeCommandBuffers(zeDevice.device(),
                             zeDevice.getCommandPool(),
                             static_cast<uint32_t >(commandBuffers.size()),
                             commandBuffers.data());
        commandBuffers.clear();
    }

    void ZeApp::recreateSwapChain() {
        auto extent = zeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = zeWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(zeDevice.device());

        if (zeSwapChain == nullptr) {
            zeSwapChain = std::make_unique<ZeSwapChain>(zeDevice, extent);
        } else {
            zeSwapChain = std::make_unique<ZeSwapChain>(zeDevice, extent, std::move(zeSwapChain));
            if (zeSwapChain->imageCount() != commandBuffers.size()) {
                freeCommanBuffers();
                createCommandsBuffers();
            }
        }
        createPipeline();
    }

    void ZeApp::drawFrame() {
        uint32_t  imageIndex;
        auto result = zeSwapChain->acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain");
        }

        recordCommandBuffer(imageIndex);
        result = zeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || zeWindow.wasWindowResized()) {
            zeWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain");

        }
    }
}