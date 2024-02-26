#pragma once

#include "ze_window.hpp"

namespace ze {

    class ZeApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;


        void run();

    private:
        ZeWindow zeWindow {WIDTH, HEIGHT, "Ze Vulkan"};
    };

}