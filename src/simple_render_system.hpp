#pragma once

#include "ze_camera.hpp"
#include "ze_pipeline.hpp"
#include "ze_device.hpp"
#include "ze_game_object.hpp"
#include "ze_frame_info.hpp"

#include <memory>
#include <vector>

namespace ze {

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(ZeDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo &frameInfo,
                               std::vector<ZeGameObject> &gameObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        ZeDevice &zeDevice;

        std::unique_ptr<ZePipeline> zePipeline;
        VkPipelineLayout  pipelineLayout;
    };

}