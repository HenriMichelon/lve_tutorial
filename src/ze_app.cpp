#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ze_camera.hpp"
#include "ze_app.hpp"
#include "ze_buffer.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"

#include <array>
#include <chrono>
#include <numeric>

namespace ze {

    struct GlobalUbo {
        alignas(16) glm::mat4 projectionView{1.0f};
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f });
    };

    ZeApp::ZeApp() {
        globalPool = ZeDescriptorPool::Builder(zeDevice)
                .setMaxSets(ZeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ZeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    }

    ZeApp::~ZeApp() {
    }

    void ZeApp::run() {
        std::vector<std::unique_ptr<ZeBuffer>> uboBuffers(ZeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<ZeBuffer>(
                    zeDevice,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = ZeDescriptorSetLayout::Builder(zeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(ZeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            ZeDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{zeDevice, zeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
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
                int frameIndex = zeRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    delta,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                zeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                zeRenderer.endSwapChainRenderPass(commandBuffer);
                zeRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(zeDevice.device());
    }


    void ZeApp::loadGameObjects() {
        std::shared_ptr<ZeModel> zeModel = ZeModel::createModelFromFile(zeDevice, "models/pumpkin_1.obj");
        auto cube = ZeGameObject::createGameObject();
        cube.model = zeModel;
        cube.transform.translation = { 0.0f, 0.2f, 1.0f };
        cube.transform.scale = glm::vec3{1.0f };
        gameObjects.push_back(std::move(cube));
    }

}