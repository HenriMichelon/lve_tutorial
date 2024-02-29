#pragma once

#include "ze_window.hpp"
#include "ze_device.hpp"
#include "ze_game_object.hpp"
#include "ze_renderer.hpp"
#include "ze_descriptors.hpp"

#include <memory>
#include <vector>

namespace ze {

    class ZeApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        ZeApp();
        ~ZeApp();

        ZeApp(const ZeApp&) = delete;
        ZeApp &operator=(const ZeApp&) = delete;

        void run();

    private:
        void loadGameObjects();

        ZeWindow zeWindow {WIDTH, HEIGHT, "Ze Vulkan"};
        ZeDevice zeDevice { zeWindow };
        ZeRenderer zeRenderer{zeWindow, zeDevice};

        // note : order of declarations matters (must be destroyed before the ZeDevice)
        std::unique_ptr<ZeDescriptorPool> globalPool{};
        std::vector<ZeGameObject> gameObjects;
    };

}