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
        glm::mat4 projectionView{1.0f};
        glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f}; // RGB + intensity
        glm::vec3 lightPosition{-1.0f};
        alignas(16) glm::vec4 lightColor{1.0f}; // RGB + intensity
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
                .addBinding(0,
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            VK_SHADER_STAGE_ALL_GRAPHICS)
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
        cameraObject.transform.translation.z = -2.0f;
        cameraObject.transform.translation.y = -1.0f;
        cameraObject.transform.rotation.x = -0.75f;
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
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                zeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                zeRenderer.endSwapChainRenderPass(commandBuffer);
                zeRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(zeDevice.device());
    }


    void ZeApp::loadGameObjects() {
        std::shared_ptr<ZeModel> zeModel = ZeModel::createModelFromFile(zeDevice, "models/pumpkin_1.obj");
        std::shared_ptr<ZeModel> zeModel1 = ZeModel::createModelFromFile(zeDevice, "models/quad.obj");

        auto gameObject1 = ZeGameObject::createGameObject();
        gameObject1.model = zeModel;
        gameObject1.transform.translation = { 0.3f, 0.5f, 0.0f };
        gameObject1.transform.scale = glm::vec3{1.2f };
        gameObjects.emplace(gameObject1.getId(), std::move(gameObject1));

        auto gameObject2 = ZeGameObject::createGameObject();
        gameObject2.model = zeModel;
        gameObject2.transform.translation = { -0.3f, 0.5f, 0.0f };
        gameObject2.transform.scale = glm::vec3{1.2f };
        gameObjects.emplace(gameObject2.getId(), std::move(gameObject2));

        auto floor = ZeGameObject::createGameObject();
        floor.model = zeModel1;
        floor.transform.translation = { 0.0f, 0.5f, 0.0f };
        floor.transform.scale = glm::vec3{1.0f };
        gameObjects.emplace(floor.getId(), std::move(floor));
    }

}