#include "ze_app.hpp"
#include <stdexcept>

namespace ze {

    ZeApp::ZeApp() {
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
        }
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

    void ZeApp::createCommandsBuffers() {}

    void ZeApp::drawFrame() {}
}