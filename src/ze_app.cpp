#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ze_camera.hpp"
#include "ze_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"

#include <array>
#include <chrono>

namespace ze {

    ZeApp::ZeApp() {
        loadGameObjects();
    }

    ZeApp::~ZeApp() {
    }

    void ZeApp::run() {
        SimpleRenderSystem simpleRenderSystem{zeDevice, zeRenderer.getSwapChainRenderPass()};
        ZeCamera camera{};

        auto cameraObject = ZeGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!zeWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime =  std::chrono::high_resolution_clock::now();
            float delta = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //delta = glm::min(delta, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(zeWindow.getGLFWwindow(), delta, cameraObject);
            camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);

            float aspect = zeRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (auto commandBuffer = zeRenderer.beginFrame()) {
                zeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                zeRenderer.endSwapChainRenderPass(commandBuffer);
                zeRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(zeDevice.device());
    }


    void ZeApp::loadGameObjects() {
        std::shared_ptr<ZeModel> zeModel = ZeModel::createModelFromFile(zeDevice, "models/wood_shield_1.obj");
        auto cube = ZeGameObject::createGameObject();
        cube.model = zeModel;
        cube.transform.translation = { 0.0f, 0.0f, 0.5f };
        cube.transform.scale = glm::vec3{5.0f };
        gameObjects.push_back(std::move(cube));
    }

}