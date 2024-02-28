
#include "ze_renderer.hpp"

#include <stdexcept>
#include <array>

namespace ze {


    ZeRenderer::ZeRenderer(ZeWindow& window, ZeDevice& device): zeWindow(window), zeDevice{device}  {
        recreateSwapChain();
        createCommandsBuffers();
    }

    ZeRenderer::~ZeRenderer() {
        freeCommanBuffers();
    }

    void ZeRenderer::createCommandsBuffers() {
        commandBuffers.resize(ZeSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = zeDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(zeDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void ZeRenderer::freeCommanBuffers() {
        vkFreeCommandBuffers(zeDevice.device(),
                             zeDevice.getCommandPool(),
                             static_cast<uint32_t >(commandBuffers.size()),
                             commandBuffers.data());
        commandBuffers.clear();
    }

    void ZeRenderer::recreateSwapChain() {
        auto extent = zeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = zeWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(zeDevice.device());

        if (zeSwapChain == nullptr) {
            zeSwapChain = std::make_unique<ZeSwapChain>(zeDevice, extent);
        } else {
            std::shared_ptr<ZeSwapChain> oldSwapChain = std::move(zeSwapChain);
            zeSwapChain = std::make_unique<ZeSwapChain>(zeDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*zeSwapChain.get())) {
                throw std::runtime_error("Swap chain image format has changed");
            }
        }
    }

    VkCommandBuffer ZeRenderer::beginFrame() {
        assert(!isFrameStarted && "can't call beginFrame while already in progress");

        auto result = zeSwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBUffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }
        return commandBuffer;
    }

    void ZeRenderer::endFrame() {
        assert(isFrameStarted && "can't call endFrame while frame not in progress");

        auto commandBuffer = getCurrentCommandBUffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffer");
        }

        auto result = zeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || zeWindow.wasWindowResized()) {
            zeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % ZeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void ZeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass while frame not in progress");
        assert(commandBuffer == getCurrentCommandBUffer() && "beginSwapChainRenderPass bad commandBuffer");

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = zeSwapChain->getRenderPass();
        renderPassBeginInfo.framebuffer = zeSwapChain->getFrameBuffer(currentImageIndex);

        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = zeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = { 1.0f,  0 };
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float >(zeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float >(zeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, zeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport (commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void ZeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "can't call endSwapChainRenderPass while frame not in progress");
        assert(commandBuffer == getCurrentCommandBUffer() && "endSwapChainRenderPass bad commandBuffer");
        vkCmdEndRenderPass(commandBuffer);
    }
}