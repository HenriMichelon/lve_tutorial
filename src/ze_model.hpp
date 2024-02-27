#pragma once

#include "ze_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace ze {
    class ZeModel {
    public:

        struct Vertex {
            glm::vec2 position;

            static std::vector<VkVertexInputBindingDescription> getBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
        };

        ZeModel(ZeDevice &device, const std::vector<Vertex> &vertices);
        ~ZeModel();

        ZeModel(const ZeModel&) = delete;
        ZeModel &operator=(const ZeModel&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        ZeDevice& zeDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t  vertexCount;

        void createVertexBuffers(const std::vector<Vertex> &vertices);
    };
}