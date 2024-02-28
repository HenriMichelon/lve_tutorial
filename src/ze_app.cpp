#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ze_app.hpp"
#include "simple_render_system.hpp"

#include <array>

namespace ze {

    ZeApp::ZeApp() {
        loadGameObjects();
    }

    ZeApp::~ZeApp() {
    }

    void ZeApp::run() {
        SimpleRenderSystem simpleRenderSystem{zeDevice, zeRenderer.getSwapChainRenderPass()};

        while (!zeWindow.shouldClose()) {
            glfwPollEvents();
            if (auto commandBuffer = zeRenderer.beginFrame()) {
                zeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                zeRenderer.endSwapChainRenderPass(commandBuffer);
                zeRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(zeDevice.device());
    }

    void ZeApp::loadGameObjects() {
        std::vector<ZeModel::Vertex> vertices {
                {{0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f }},
                {{0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f }},
                {{-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f }}
        };
        auto zeModel = std::make_shared<ZeModel>(zeDevice, vertices);
        auto triangle = ZeGameObject::createGameObject();
        triangle.model = zeModel;
        triangle.color = { 0.1f, 0.8f, 0.1f };
        triangle.transform2D.translation.x = 0.2f;
        triangle.transform2D.scale = { 2.0f, 0.5 };
        triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

}