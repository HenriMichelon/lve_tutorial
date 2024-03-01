#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "point_light_system.hpp"

#include <stdexcept>
#include <array>

namespace ze {

    PointLightSystem::PointLightSystem(ZeDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout): zeDevice{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(zeDevice.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
//        VkPushConstantRange pushConstantRange{};
//        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//        pushConstantRange.offset = 0;
//        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(zeDevice.device(), &pipelineLayoutCreateInfo, nullptr,&pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before layout");

        PipelineConfigInfo pipelineConfigInfo{};
        ZePipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
        pipelineConfigInfo.renderPass = renderPass;
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        zePipeline = std::make_unique<ZePipeline>(
                zeDevice,
                "shaders/point_light.vert.spv",
                "shaders/point_light.frag.spv",
                pipelineConfigInfo
                );
    }

    void PointLightSystem::render(FrameInfo &frameInfo) {
        zePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &frameInfo.globalDescriptorSet,
                0,
                nullptr
                );
        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }

}