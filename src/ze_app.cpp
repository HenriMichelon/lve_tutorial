#include "ze_app.hpp"

namespace ze {
    void ZeApp::run() {
        while (!zeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}