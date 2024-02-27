#include "ze_app.hpp"
#include <stdexcept>
#include <array>

namespace ze {

    ZeApp::ZeApp() {
        loadModels();
        createPipelineLayout();
        createPipeline();
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
        auto pipelineConfig = ZePipeline::defaultPipelineConfigInfo(zeSwapChain.width(), zeSwapChain.height());
        pipelineConfig.renderPass = zeSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        zePipeline = std::make_unique<ZePipeline>(
                zeDevice,
                "shaders/simple_shader.vert.spv",
                "shaders/simple_shader.frag.spv",
                pipelineConfig
                );
    }

    void ZeApp::createCommandsBuffers() {
        commandBuffers.resize(zeSwapChain.imageCount());
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = zeDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(zeDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }
            VkRenderPassBeginInfo renderPassBeginInfo{};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = zeSwapChain.getRenderPass();
            renderPassBeginInfo.framebuffer = zeSwapChain.getFrameBuffer(i);

            renderPassBeginInfo.renderArea.offset = {0, 0};
            renderPassBeginInfo.renderArea.extent = zeSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f};
            clearValues[1].depthStencil = { 1.0f,  0 };
            renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            zePipeline->bind(commandBuffers[i]);
            zeModel->bind(commandBuffers[i]);
            zeModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to end command buffer");

            }
        }
    }

    void ZeApp::drawFrame() {
        uint32_t  imageIndex;
        auto result = zeSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain");
        }

        result = zeSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain");

        }
    }
}