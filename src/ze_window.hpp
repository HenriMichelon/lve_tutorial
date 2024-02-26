#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

 namespace ze {
    class ZeWindow {
    public:
        ZeWindow(int w, int h, std::string name);
        ~ZeWindow();

        ZeWindow(const ZeWindow &) = delete;
        ZeWindow &operator=(const ZeWindow&) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        void initWindow();

        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;
    };
}