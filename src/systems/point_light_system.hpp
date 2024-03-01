#pragma once

#include "../ze_camera.hpp"
#include "../ze_pipeline.hpp"
#include "../ze_device.hpp"
#include "../ze_game_object.hpp"
#include "../ze_frame_info.hpp"

#include <memory>
#include <vector>

namespace ze {

    class PointLightSystem {
    public:
        PointLightSystem(ZeDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem &operator=(const PointLightSystem&) = delete;

        void render(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        ZeDevice &zeDevice;

        std::unique_ptr<ZePipeline> zePipeline;
        VkPipelineLayout  pipelineLayout;
    };

}