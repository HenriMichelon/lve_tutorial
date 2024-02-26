#pragma once

#include "ze_window.hpp"
#include "ze_pipeline.hpp"
#include "ze_device.hpp"

namespace ze {

    class ZeApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;


        void run();

    private:
        ZeWindow zeWindow {WIDTH, HEIGHT, "Ze Vulkan"};
        ZeDevice zeDevice { zeWindow };

        ZePipeline zePipeline{zeDevice,
                              "shaders/simple_shader.vert.spv",
                              "shaders/simple_shader.frag.spv",
                              ZePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };

}