#include "ze_app.hpp"
#include <stdexcept>
#include <array>

namespace ze {

    ZeApp::ZeApp() {
        loadModels();
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

    void ZeApp::loadModels() {
        std::vector<ZeModel::Vertex> vertices {
                {{0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f }},
                {{0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f }},
                {{-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f }}
        };
        zeModel = std::make_unique<ZeModel>(zeDevice, vertices);
    }

    void ZeApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
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
        clearValues[0].color = {0.1f, 0.1f, 0.1f};
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

        zePipeline->bind(commandBuffers[imageIndex]);
        zeModel->bind(commandBuffers[imageIndex]);
        zeModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffer");

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