#pragma once
#include "ze_device.hpp"

#include <string>
#include <vector>

namespace ze {

    struct PipelineConfigInfo {

    };

    class ZePipeline {
    public:
        ZePipeline(ZeDevice &device,
                   const std::string& vertFilePath,
                   const std::string& fragFilePath,
                   const PipelineConfigInfo &configInfo);
        ~ZePipeline() {};

        ZePipeline(const ZePipeline&) = delete;
        void operator=(const ZePipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(const std::string& vertFilePath,
                                    const std::string& fragFilePath,
                                    const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char>&code, VkShaderModule* shaderModule);

        ZeDevice& zeDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}