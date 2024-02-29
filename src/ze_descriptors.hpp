#pragma once

#include "ze_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ze {

    class ZeDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(ZeDevice &device) : zeDevice{device} {}

            Builder &addBinding(
                    uint32_t binding,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);
            std::unique_ptr<ZeDescriptorSetLayout> build() const;

        private:
            ZeDevice &zeDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        ZeDescriptorSetLayout(
                ZeDevice &ZeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~ZeDescriptorSetLayout();
        ZeDescriptorSetLayout(const ZeDescriptorSetLayout &) = delete;
        ZeDescriptorSetLayout &operator=(const ZeDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        ZeDevice &zeDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class ZeDescriptorWriter;
    };

    class ZeDescriptorPool {
    public:
        class Builder {
        public:
            Builder(ZeDevice &device) : zeDevice{device} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<ZeDescriptorPool> build() const;

        private:
            ZeDevice &zeDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        ZeDescriptorPool(
                ZeDevice &device,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~ZeDescriptorPool();
        ZeDescriptorPool(const ZeDescriptorPool &) = delete;
        ZeDescriptorPool &operator=(const ZeDescriptorPool &) = delete;

        bool allocateDescriptor(
                const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        ZeDevice &zeDevice;
        VkDescriptorPool descriptorPool;

        friend class ZeDescriptorWriter;
    };

    class ZeDescriptorWriter {
    public:
        ZeDescriptorWriter(ZeDescriptorSetLayout &setLayout, ZeDescriptorPool &pool);

        ZeDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        ZeDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        ZeDescriptorSetLayout &setLayout;
        ZeDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace lve