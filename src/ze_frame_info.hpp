#pragma once

#include "ze_camera.hpp"
#include "ze_game_object.hpp"

#include <vulkan/vulkan.h>

namespace ze {

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        ZeCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        ZeGameObject::Map &gameObjects;
    };

}