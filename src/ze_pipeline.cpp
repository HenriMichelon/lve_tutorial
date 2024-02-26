#include "ze_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace ze {
    ZePipeline::ZePipeline(const std::string &vertFilePath, const std::string &fragFilePath) {
        createGraphicsPipeline(vertFilePath, fragFilePath);
    }

    void ZePipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath) {
        auto vertCode = readFile(vertFilePath);
        auto fragCode = readFile(fragFilePath);

        std::cout << "Vertex code size: " << vertCode.size() << '\n';
        std::cout << "Fragment code size: " << fragCode.size() << '\n';
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