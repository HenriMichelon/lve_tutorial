#pragma once

#include "ze_device.hpp"
#include "ze_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace ze {
    class ZeModel {
    public:

        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

            bool operator==(const Vertex&other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void loadModel(const std::string &filepath);
        };

        ZeModel(ZeDevice &device, const ZeModel::Builder &builder);
        ~ZeModel();

        static std::unique_ptr<ZeModel> createModelFromFile(ZeDevice &device, const std::string &filepath);

        ZeModel(const ZeModel&) = delete;
        ZeModel &operator=(const ZeModel&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        ZeDevice& zeDevice;

            std::unique_ptr<ZeBuffer> vertexBuffer;
            uint32_t  vertexCount;

            bool hasIndexBuffer{false};
            std::unique_ptr<ZeBuffer> indexBuffer;
            uint32_t  indexCount;

    };
}