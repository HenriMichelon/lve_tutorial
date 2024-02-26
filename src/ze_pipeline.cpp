#include "ze_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace ze {
    ZePipeline::ZePipeline(ZeDevice &device, const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &configInfo): zeDevice{device} {
        createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }

    void ZePipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &configInfo) {
        auto vertCode = readFile(vertFilePath);
        auto fragCode = readFile(fragFilePath);

        std::cout << "Vertex code size: " << vertCode.size() << '\n';
        std::cout << "Fragment code size: " << fragCode.size() << '\n';
    }

    void ZePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
        if (vkCreateShaderModule(zeDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
    }

    PipelineConfigInfo ZePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo{};
        return configInfo;
    }

    std::vector<char> ZePipeline::readFile(const std::string& filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};
        if (!file.is_open()) {
            throw std::runtime_error("filed to open file : " + filepath);
        }
        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
}