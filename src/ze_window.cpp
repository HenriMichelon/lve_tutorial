#include "ze_window.hpp"

namespace ze {
    ZeWindow::ZeWindow(int w, int h, std::string name): width{w}, height{h}, windowName{name} {
        initWindow();
    }

    ZeWindow::~ZeWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void ZeWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }
}