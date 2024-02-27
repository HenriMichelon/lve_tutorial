#pragma once

#include "ze_window.hpp"
#include "ze_pipeline.hpp"
#include "ze_device.hpp"
#include "ze_swap_chain.hpp"
#include "ze_model.hpp"

#include <memory>
#include <vector>

namespace ze {

    class ZeApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        ZeApp();
        ~ZeApp();

        ZeApp(const ZeApp&) = delete;
        ZeApp &operator=(const ZeApp&) = delete;

        void run();

    private:
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandsBuffers();
        void drawFrame();

        ZeWindow zeWindow {WIDTH, HEIGHT, "Ze Vulkan"};
        ZeDevice zeDevice { zeWindow };
        ZeSwapChain zeSwapChain { zeDevice, zeWindow.getExtent() };

        std::unique_ptr<ZePipeline> zePipeline;
        VkPipelineLayout  pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<ZeModel> zeModel;

        void sierpinski( std::vector<ZeModel::Vertex> &vertices,
                         int depth,
                         glm::vec2 left,
                         glm::vec2 right,
                         glm::vec2 top);

    };

}