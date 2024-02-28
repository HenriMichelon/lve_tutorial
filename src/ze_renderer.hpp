#pragma once

#include "ze_window.hpp"
#include "ze_device.hpp"
#include "ze_swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace ze {

    class ZeRenderer {
    public:
        ZeRenderer(ZeWindow& zeWindow, ZeDevice& zeDevice);
        ~ZeRenderer();

        ZeRenderer(const ZeRenderer&) = delete;
        ZeRenderer &operator=(const ZeRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return zeSwapChain->getRenderPass(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBUffer() const {
            assert(isFrameStarted && "cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandsBuffers();
        void freeCommanBuffers();
        void recreateSwapChain();

        ZeWindow& zeWindow;
        ZeDevice& zeDevice;

        std::unique_ptr<ZeSwapChain> zeSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex{0};
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}