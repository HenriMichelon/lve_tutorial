#include "ze_window.hpp"
#include <stdexcept>
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void ZeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void ZeWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto zeWindow = reinterpret_cast<ZeWindow*>(glfwGetWindowUserPointer(window));
        zeWindow->framebufferResized = true;
        zeWindow->width = width;
        zeWindow->height = height;
    }
}